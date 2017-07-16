#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <dht.h>

dht DHT;

#define MOISTURE 0
#define DHT11PIN 2
#define BACKLIGHT_PIN     13

LiquidCrystal_I2C lcd(0x38, 2, 1, 0, 4,5,6,7,3,POSITIVE);  // Set the LCD I2C address

void setup()
{
  Serial.begin(9600);
  
  lcd.begin(16,2);               // initialize the lcd 

  lcd.home();                   // go home
  lcd.print("    LOADING    ");

  delay(1000);
  lcd.clear();
}

void loop()
{
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
    break;
  }

  Serial.print("Humidity (%): ");
  Serial.println((float)DHT.humidity, 2);

  Serial.print("Temperature (°C): ");
  Serial.println((float)DHT.temperature, 2);

  int moisture = map(analogRead(MOISTURE), 0, 1023, 99, 0);
  Serial.print("Moisture (%): ");
  Serial.println(moisture);

  char buffer[16];

  //lcd.clear();
  lcd.home();
  lcd.print((int)DHT.temperature);
  lcd.setCursor(3,0);
  lcd.print("\xDF");lcd.print("C");

  sprintf(buffer, "%2d",(int)DHT.humidity);
  lcd.setCursor(0,1);
  lcd.print(buffer);
  lcd.setCursor(3,1);
  lcd.print("%H");

  sprintf(buffer, "%2d",moisture);
  lcd.setCursor(7,1);
  lcd.print(buffer);
  lcd.setCursor(10,1);
  lcd.print("%M");
  
  delay(100);
}
