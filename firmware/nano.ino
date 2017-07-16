#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <dht.h>

// Uncomment this for debug
#define _DEBUG_
#define TINY_GSM_DEBUG Serial
#define TINY_GSM_RX_WAIT 1500
#define BAUD 38400

// Can be installed from Library Manager or https://github.com/vshymanskyy/TinyGSM
#include <TinyWiFiClientESP8266.h>
#include <ThingerESP8266AT.h>

#define USERNAME "theaspect"
#define DEVICE_ID "nano"
#define DEVICE_CREDENTIAL "w9xLpKy7TO4s"

#define SSID "iHome"
#define SSID_PASSWORD "A2B3R9QA"

#define MOISTURE 0
#define DHT11PIN 2
#define BACKLIGHT_PIN 13

// Emulate Serial1 on pins 10/11 if HW is not present (use interrupt pins for better performance)
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(10,11); // RX, TX
#endif

dht DHT;
LiquidCrystal_I2C lcd(0x38,2,1,0,4,5,6,7,3,POSITIVE);  // Set the LCD I2C address
ThingerESP8266AT thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL, Serial1);

int temperature = 99;
int humidity = 99;
int moisture = 99;

int target_temperature = 99;
int target_humidity = 99;
int target_moisture = 99;

unsigned long last_update = 0;

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
//  int chk = DHT.read11(DHT11PIN);
//
//  Serial.print("Read sensor: ");
//  switch (chk)
//  {
//    case DHTLIB_OK: 
//   Serial.println("OK"); 
//    break;
//    case DHTLIB_ERROR_CHECKSUM: 
//    Serial.println("Checksum error"); 
//    break;
//    case DHTLIB_ERROR_TIMEOUT: 
//    Serial.println("Time out error"); 
//    break;
//    default: 
//    Serial.println("Unknown error");
//    //delay(10);
//    break;
//  }
//
//  humidity=(int) DHT.humidity;
//  temperature=(int) DHT.temperature;

//  Serial.print("Humidity (%): ");
//  Serial.println(humidity);
//
//  Serial.print("Temperature (°C): ");
//  Serial.println(temperature);

  moisture = map(analogRead(MOISTURE), 0, 1023, 99, 0);
//  Serial.print("Moisture (%): ");
//  Serial.println(moisture);
}

void setup()
{
  Serial.begin(BAUD);
  /* Notice: initialize serial for ESP8266 at your ESP8266 baud rate
   * You can change the baud rate of ESP8266 sending a command like "AT+UART_DEF=9600,8,1,0,0\r\n"
   * Test with different rates, and use the higher one that works with your setup.
   */
  Serial1.begin(BAUD);
  thing.add_wifi(SSID, SSID_PASSWORD);
  thing["temperature"] >> outputValue(temperature);
  thing["humidity"] >> outputValue(humidity);
  thing["moisture"] >> outputValue(moisture);

//  thing["target_temperature"] << inputValue(target_temperature);
//  thing["target_humidity"] << inputValue(target_humidity);
//  thing["target_moisture"] << inputValue(target_moisture);

  lcd.begin(16,2);               // initialize the lcd 

  lcd.home();                   // go home
  //lcd.print("    LOADING    ");

  //delay(1000);
  //lcd.clear();
}

void loop()
{
  if(millis() - last_update > 10000){
//    DBG(last_update);
    last_update = millis();
    read_sensors();
//    print_lcd();
  }else{
    thing.handle();
  }
  //delay(50);
}
