
#define TINY_GSM_MODEM_BG96  //works for Quectel EC200U!
#define SerialAT Serial1
#define SerialMon Serial
#define TINY_GSM_USE_GPRS true
#include <TinyGsmClient.h>
#include <PubSubClient.h>

#define RXD1 40    //ESP32-S3 pin 40 is internally connected to Tx of the 4G Module
#define TXD1 41    //ESP32-S3 pin 41 is internally connected to Rx of the 4G Module
#define powerPin 42 ////ESP32-S3 pin 42 is internally connected to power pin of 4G Module

const char apn[]      = ""; //APN automatically detects for 4G SIM in most cases, else set APN provided by your Network Operator For Jio, APN is jionet
const char *broker         = "broker.hivemq.com"; // REPLACE IF YOU ARE USING ANOTHER BROKER
const char *topic        = "TestTopicVVM601";     //PUBLISH TOPIC TO SEND MESSAGE EVERY 3 SECONDS. REPLACE XXXXX WITH YOUR TOPIC NAME

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif
TinyGsmClient gsmClient(modem);
PubSubClient  mqtt(gsmClient);

void setup()
{
  Serial.begin(115200);
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);
  delay(100);
  digitalWrite(powerPin, HIGH);
  delay(1000);
  digitalWrite(powerPin, LOW);

  Serial.println("\nconfiguring VVM601 Module. Kindly wait");

  delay(5000);
  SerialAT.begin(115200, SERIAL_8N1, RXD1, TXD1);
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DBG("Initializing modem...");
  if (!modem.init()) {
    DBG("Failed to restart modem, delaying 5s and retrying");
    return;
  }
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DBG("Initializing modem...");
  if (!modem.restart()) {
    DBG("Failed to restart modem, delaying 10s and retrying");
    return;
  }

  String name = modem.getModemName();
  DBG("Modem Name:", name);

  String modemInfo = modem.getModemInfo();
  DBG("Modem Info:", modemInfo);


  Serial.println("Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" success");

  if (modem.isNetworkConnected()) {
    Serial.println("Network connected");
  }


  // GPRS connection parameters are usually set after network registration
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn)) {
    Serial.println(" fail");
    delay(5000);
    return;
  }
  Serial.println(" success");

  if (modem.isGprsConnected())
  {
    Serial.println("LTE module connected");
  }


  // MQTT Broker setup
  mqtt.setServer(broker, 1883);  //broker domain name and port number, 8883 is for Secure MQTT
  //mqtt.setCallback(callback);

}


void loop()
{
  if (!modem.isNetworkConnected())
  {
    Serial.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true)) {
      Serial.println(" fail");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected()) {
      Serial.println("Network re-connected");
    }

#if TINY_GSM_USE_GPRS
    // and make sure GPRS/EPS is still connected
    if (!modem.isGprsConnected()) {
      Serial.println("GPRS disconnected!");
      Serial.print(F("Connecting to "));
      Serial.print(apn);
      if (!modem.gprsConnect(apn)) {
        Serial.println(" fail");
        delay(10000);
        return;
      }
      if (modem.isGprsConnected()) {
        Serial.println("GPRS reconnected");
      }
    }
#endif
  }

  if (!mqtt.connected()) {
    reconnect();  // Reconnect only if not connected
  }

  if (mqtt.connected()) 
  {
    if(mqtt.publish(topic, "Hello...Data Transmitted"))
    {
      Serial.println("Published to broker");
    }
    else
    {
       Serial.println("Failed to publish to broker");
    }
  }
  mqtt.loop();  // Handle MQTT connection
  delay(5000);  // Delay after publishing (adjust as needed)
}


void reconnect() {
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    boolean status = mqtt.connect("VVM601_67354"); // use unique client ID

    if (status == false) {
      Serial.print(" failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    } else {
      Serial.println(" success");
      // mqtt.subscribe(led_on_off);
    }
  }
}
