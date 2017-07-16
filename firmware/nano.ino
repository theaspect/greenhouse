#include <LiquidCrystal_I2C.h>
#include <dht.h>

#define BAUD 38400

#define WIFI_SSID "iHome"
#define WIFI_PASSWORD "A2B3R9QA"

#define MOISTURE 0
#define DHT11PIN 2
#define BACKLIGHT_PIN 13

// Emulate Serial1 on pins 10/11 if HW is not present (use interrupt pins for better performance)
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial SerialWiFi(10,11); // RX, TX
#endif

dht DHT;
LiquidCrystal_I2C lcd(0x38,2,1,0,4,5,6,7,3,POSITIVE);  // Set the LCD I2C address

#define IP "192.168.1.101"
#define PORT 8080

unsigned long lastResetTime = 0;
const unsigned long resetInterval = 60000L;
unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10000L; // delay between updates, in milliseconds

uint8_t data[256];

int temperature = 99;
int humidity = 99;
int moisture = 99;

int target_temperature = 99;
int target_humidity = 99;
int target_moisture = 99;

void print_lcd(){
  char buffer[16];

  //lcd.clear();
  lcd.home();
  lcd.print(temperature);
  lcd.setCursor(3,0);
  lcd.print("\xDF");lcd.print("C");

  sprintf(buffer, "%2d",humidity);
  lcd.setCursor(0,1);
  lcd.print(buffer);
  lcd.setCursor(3,1);
  lcd.print("%H");

  sprintf(buffer, "%2d",moisture);
  lcd.setCursor(7,1);
  lcd.print(buffer);
  lcd.setCursor(10,1);
  lcd.print("%M");
}

void read_sensors(){
  int chk = DHT.read11(DHT11PIN);

  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
   Serial.println("OK"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.println("Checksum error"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.println("Time out error"); 
    break;
    default: 
    Serial.println("Unknown error");
    //delay(10);
    break;
  }

  humidity=(int) DHT.humidity;
  temperature=(int) DHT.temperature;

  Serial.print("Humidity (%): ");
  Serial.println(humidity);

  Serial.print("Temperature (°C): ");
  Serial.println(temperature);

  moisture = map(analogRead(MOISTURE), 0, 1023, 99, 0);
  Serial.print("Moisture (%): ");
  Serial.println(moisture);
}

void reset() {
  SerialWiFi.println("AT+RST");
  Serial.println("AT+RST");
  delay(1000);
  if(SerialWiFi.find("OK")){
    Serial.println("Module Reset"); 
  }else{
    Serial.println("Failde to reset"); 
  }
}

void connectWifi() {
  String cmdm = "AT+CWMOD=1";
  Serial.println(cmdm);
  SerialWiFi.println(cmdm);
  delay(100);
  if(SerialWiFi.find("OK")) {
    Serial.println("Mod changed");
  } else {
    Serial.println("Cannot change mod");
  }
  
  String cmd = "AT+CWJAP=\"" + String(WIFI_SSID)+"\",\"" + String(WIFI_PASSWORD) + "\"";
  Serial.println(cmd);
  SerialWiFi.println(cmd);

  delay(4000);

  if(SerialWiFi.find("OK")) {
    Serial.println("Connected!");
  } else {
    Serial.println("Cannot connect to wifi");
    connectWifi();
  }
}

void http (String url) {
  String connect = "AT+CIPSTART=\"TCP\",\"" + String(IP) + "\"," + String(PORT);
  Serial.println(connect);
  SerialWiFi.println(connect);//start a TCP connection.
  if( SerialWiFi.find("OK")) {
    Serial.println("TCP connection ready");
  }else{
    Serial.println("Cannot connect");
  }

  delay(100);

  String getRequest =
    "GET " + url + " HTTP/1.1\r\n" +
    "Host: " + IP + "\r\n" +
    "Connection: close" +
    "\r\n\r\n";

  String sendCmd = "AT+CIPSEND="+String(getRequest.length());//determine the number of caracters to be sent.
  
  Serial.println(sendCmd);
  SerialWiFi.println(sendCmd);

  delay(500);
  if(SerialWiFi.find(">")) { 
    Serial.println("Sending..");
    SerialWiFi.print(getRequest);
  }else{
    Serial.println("Failed to start");
  }

  delay(100);
  if(SerialWiFi.find("SEND OK")) {
    Serial.println("Packet sent");
  }else{
    Serial.println("Failed to send");
  }

  read();
  
  // close the connection
  SerialWiFi.println("AT+CIPCLOSE");
  Serial.println("AT+CIPCLOSE");
  delay(100);
  if(SerialWiFi.find("ERROR")) {
    Serial.println("Closed");
  }else{
    Serial.println("Failed to close");
  }
}

void read(){
  while (SerialWiFi.available()) {
    String tmpResp = SerialWiFi.readString();
    Serial.println(tmpResp);
  }
}

void setup()
{
  Serial.begin(BAUD);
  /* Notice: initialize serial for ESP8266 at your ESP8266 baud rate
   * You can change the baud rate of ESP8266 sending a command like "AT+UART_DEF=9600,8,1,0,0\r\n"
   * Test with different rates, and use the higher one that works with your setup.
   */
  SerialWiFi.begin(BAUD);
  
  lcd.begin(16,2);               // initialize the lcd 

  lcd.home();                   // go home
  //lcd.print("    LOADING    ");

  //delay(1000);
  //lcd.clear();
}

void loop()
{
  if (millis() - lastResetTime > resetInterval) {
      reset();
      connectWifi();
      lastResetTime = millis();
  }

  // if 10 seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval) {
    read_sensors();
    print_lcd();

    String url = "/log?moisture="+String(moisture)+"&humidity="+String(humidity)+"&temperature="+String(temperature);
    http(url);
    lastConnectionTime = millis();
  }
  delay(1000);
}
