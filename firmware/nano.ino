#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <dht.h>

dht DHT;

#define MOISTURE 0
#define DHT11PIN 2
#define BACKLIGHT_PIN     13

LiquidCrystal_I2C lcd(0x38, 2, 1, 0, 4,5,6,7,3,POSITIVE);  // Set the LCD I2C address

//LiquidCrystal_I2C lcd(0x38, BACKLIGHT_PIN, POSITIVE);  // Set the LCD I2C address


// Creat a set of new characters
const uint8_t charBitmap[][8] = {
   { 0xc, 0x12, 0x12, 0xc, 0, 0, 0, 0 },
   { 0x6, 0x9, 0x9, 0x6, 0, 0, 0, 0 },
   { 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0, 0x0 },
   { 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0, 0x0 },
   { 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0x0 },
   { 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0x0 },
   { 0x0, 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0x0 },
   { 0x0, 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0x0 }
   
};

void setup()
{
  Serial.begin(9600);
  Serial.println("DHT11 TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  
   int charBitmapSize = (sizeof(charBitmap ) / sizeof (charBitmap[0]));

  // Switch on the backlight
  pinMode ( BACKLIGHT_PIN, OUTPUT );
  digitalWrite ( BACKLIGHT_PIN, HIGH );
  
  lcd.begin(16,2);               // initialize the lcd 

   for ( int i = 0; i < charBitmapSize; i++ )
   {
      lcd.createChar ( i, (uint8_t *)charBitmap[i] );
   }

  lcd.home ();                   // go home
  lcd.print("Hello, ARDUINO ");  
  lcd.setCursor ( 0, 1 );        // go to the next line
  lcd.print (" FORUM - fm   ");
  delay ( 1000 );
}

void loop()
{
   lcd.home ();
   // Do a little animation by writing to the same location
   for ( int i = 0; i < 2; i++ )
   {
      for ( int j = 0; j < 16; j++ )
      {
         lcd.print (char(random(7)));
      }
      lcd.setCursor ( 0, 1 );
   }
   delay (200);

   Serial.println("\n");

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
  
}
