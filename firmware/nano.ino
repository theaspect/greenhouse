#include <LiquidCrystal_I2C.h>
#include <dht.h>

#define _ESPLOGLEVEL_ 4
#include "WiFiEsp.h"

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
int status = WL_IDLE_STATUS;
IPAddress ip(192,168,1,101);
uint16_t port = 8080;

String ip_string = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10000L; // delay between updates, in milliseconds

// Initialize the Ethernet client object
WiFiEspClient client;

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

void setup()
{
  Serial.begin(BAUD);
  /* Notice: initialize serial for ESP8266 at your ESP8266 baud rate
   * You can change the baud rate of ESP8266 sending a command like "AT+UART_DEF=9600,8,1,0,0\r\n"
   * Test with different rates, and use the higher one that works with your setup.
   */
  SerialWiFi.begin(BAUD);

  // initialize ESP module
  WiFi.init(&SerialWiFi);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  Serial.println("You're connected to the network");
  
  printWifiStatus();

  lcd.begin(16,2);               // initialize the lcd 

  lcd.home();                   // go home
  //lcd.print("    LOADING    ");

  //delay(1000);
  //lcd.clear();
}

void loop()
{
  // if there's incoming data from the net connection send it out the serial port
  // this is for debugging purposes only
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if 10 seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval) {
    read_sensors();
    print_lcd();
  
    httpRequest();
  }

  delay(1000);
}

// this method makes a HTTP connection to the server
void httpRequest() {
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();
  client.flush();

  // if there's a successful connection
  if (client.connect(ip, port)) {
    // send the HTTP GET request

    sprintf(data, 
      "GET /log?moisture=%d&humidity=%d&temperature=%d HTTP/1.1\r\nHost: %d.%d.%d.%d\r\nConnection: close\r\n\r\n",
      moisture,humidity,temperature,ip[0],ip[1],ip[2],ip[3]);
    int len = strlen(data);

    Serial.print("SENDING ");
    Serial.print(len);
    Serial.print(" :");
    Serial.write(data, len);
    client.write(data, len);
    Serial.println("READING");
    client.read();

//    delay(100);
//
//    while (client.available()) {
//      char c = client.read();
//      Serial.write(c);
//    }

    // note the time that the connection was made
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

