// THIS EXAMPLE IS USED FOR AT COMMAND TESTING
// YOU CAN WRITE THE AT COMMANDS IN THE SERIAL PORT TO VIEW THE RESPONSES, SET BAUD RATE to 115200
// SELECT SERIAL PORT MONITOR AS "BOTH NL & CR" TO VIEW COMMAND RESPONSE CORRECTLY IF YOU ARE USING ARDUINO IDE
//FOR VVM501 PRODUCT DETAILS VISIT www.vv-mobility.com

#define SerialAT Serial1
#define SerialMon Serial
#define RXD1 40    //ESP32-S3 pin 40 is internally connected to Tx of the 4G Module
#define TXD1 41    //ESP32-S3 pin 41 is internally connected to Rx of the 4G Module
#define powerPin 42 ////ESP32-S3 pin 42 is internally connected to power pin of 4G Module
char a, b;
void setup() {
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);
  Serial.begin(115200);
  delay(100);
  Serial.println("Waiting for modem response");
  SerialAT.begin(115200, SERIAL_8N1, RXD1, TXD1);
  delay(5000); // DELAY FOR MODEM TO SET
  Serial.println("Modem Reset, Please Wait");
  SerialAT.println("AT+RESET");
  delay(1000);
  SerialAT.println("AT+RESET");
  delay(10000);
  //////
 SerialAT.println("AT+CREG?");
 delay(1000);
  SerialAT.flush();
  Serial.println("Enter Standard AT commands like AT, AT+CPIN?, AT+CCLK?, etc.");
  Serial.println("SELECT SERIAL PORT MONITOR AS \"BOTH NL & CR\" TO VIEW COMMAND RESPONSE CORRECTLY IF YOU ARE USING ARDUINO IDE");
  Serial.println("Refer A7600 series datasheet for entire list of commands");
  Serial.println("Understand the AT Commands properly");
  Serial.println("Incorrect AT commands can corrupt the 4G module memory!!!");

  
}

void loop()
{
  if (Serial.available() > 0) // read AT commands from user Serial port and send to the Module
  {
    a = Serial.read();
    SerialAT.write(a);
  }
  if (SerialAT.available() > 0) //read Response commands from module and send to user Serial Port
  {
    b = SerialAT.read();
    Serial.write(b);
  }
}