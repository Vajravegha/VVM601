
#define BLYNK_AUTH_TOKEN "LXXXXXXXXXXXXXXXXXXXXXXXXXXXl" // enter your token here


#define TINY_GSM_MODEM_BG96  //works for Quectel EC200U!
#define SerialAT Serial1
#define SerialMon Serial

#define RXD1 40    //ESP32-S3 pin 40 is internally connected to Tx of the 4G Module
#define TXD1 41    //ESP32-S3 pin 41 is internally connected to Rx of the 4G Module
#define powerPin 42 ////ESP32-S3 pin 42 is internally connected to power pin of 4G Module
#define TINY_GSM_USE_GPRS true
unsigned int counter = 0;
int rx = -1;
String rxString;

#include <BlynkSimpleTinyGSM.h>
BlynkTimer timer;

char auth[] = BLYNK_AUTH_TOKEN;
char apn[]  = "";
char user[] = "";
char pass[] = "";

TinyGsm modem(SerialAT);

void setup()
{
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);
  delay(100);
  digitalWrite(powerPin, HIGH);
  delay(1000);
  digitalWrite(powerPin, LOW);
  Serial.begin(115200);
  delay(100);
  SerialAT.begin(115200, SERIAL_8N1, RXD1, TXD1);
  delay(5000);
  Serial.println("Modem Reset, Please Wait");
  SerialAT.println("AT+CRESET");
  delay(1000);
  SerialAT.println("AT+CRESET");
  delay(5000);
  SerialAT.flush();
  Serial.println("Echo Off");
  SerialAT.println("ATE0");   //120s
  delay(1000);
  SerialAT.println("ATE0");   //120s
  rxString = SerialAT.readString();
  Serial.print("Got: ");
  Serial.println(rxString);
  rx = rxString.indexOf("OK");
  if (rx != -1)
    Serial.println("Modem Ready");
  delay(1000);

  Serial.println("SIM card check");
  SerialAT.println("AT+CPIN?"); //9s
  rxString = SerialAT.readString();
  Serial.print("Got: ");
  Serial.println(rxString);
  rx = rxString.indexOf("+CPIN: READY");
  if (rx != -1)
    Serial.println("SIM Card Ready");
  delay(1000);

  String modemInfo = modem.getModemInfo();
  //Serial.print("Modem Info: ");
  //Serial.println(modemInfo);
  Serial.println("Trying Blynk");
  Blynk.begin(auth, modem, apn, user, pass, "blynk.cloud", 80);
}

void loop()
{
  Serial.println("Loop");
  delay(1000);
  Blynk.virtualWrite(V1, counter); // for blink application
  delay(2000);
  counter++;

  Blynk.run();
  timer.run();
}
