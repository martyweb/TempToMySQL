/***************************************************************************
  UPDATED: Tweaked to work with BME280 and OLED screen: @martywassmer
  
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <WiFi101.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_SSD1306 display = Adafruit_SSD1306();
Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

#if defined(ESP8266)
  #define BUTTON_A 0
  #define BUTTON_B 16
  #define BUTTON_C 2
  #define LED      0
#elif defined(ARDUINO_STM32F2_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
  #define LED PB5
#elif defined(TEENSYDUINO)
  #define BUTTON_A 4
  #define BUTTON_B 3
  #define BUTTON_C 8
  #define LED 13
#else 
  #define BUTTON_A 9
  #define BUTTON_B 6
  #define BUTTON_C 5
  #define LED      13
#endif

char ssid[] = "****"; //  your network SSID (name)
char pass[] = "****";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
char server[] = "www.martyweb.com"; 
WiFiClient client;

#if (SSD1306_LCDHEIGHT != 32)
 #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int sel;

void setup() {

  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("BME280, OLED, and WIFI test");

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
//  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(500);

  //attempt to connect
  if(!connectToWIFI()) sendToDisplay("Error connecting", 2000, true);
  
 
 
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  
}

void sendToServer(float temp, float alt, float pres, float hum){
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.print("GET /adafruitTemp/index.php?tmp=");
    client.print(temp);
    client.print("&alt=");
    client.print(alt);
    client.print("&pres=");
    client.print(pres);
    client.print("&hum=");
    client.print(hum);
    client.println(" HTTP/1.1");
    client.println("Host: www.martyweb.com");
    client.println("Connection: close");
    client.println();
  }

  

}

void sendToDisplay(String message, int delayTime, bool clearDisplay){
  if(clearDisplay)display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  if(clearDisplay)display.setCursor(0,0);
  display.print(message);
  display.display(); // actually display all of the above
  delay(delayTime);
}

bool connectToWIFI(){
 
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  
  // attempt to connect to Wifi network:
  int x = 0;
  while (status != WL_CONNECTED) {
    if(x>=5)return false; //try x times before giving up
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    sendToDisplay("Connecting to ", 0, true);
    sendToDisplay(ssid, 2000, false);
    
    
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    x++;
    delay(5000); // wait x seconds for connection:
  }
  Serial.println("Connected to wifi");

  sendToDisplay("Connected to ", 0, true);
  sendToDisplay(ssid, 1000, false);
  
  printWifiStatus();
  return true;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  String ip_str = String(ip[0]) + String(".") +\
  String(ip[1]) + String(".") +\
  String(ip[2]) + String(".") +\
  String(ip[3])  ;
  
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  sendToDisplay("IP ", 0, true);
  sendToDisplay(ip_str, 4000, false);
  
}

void loop() {

  float temp = ((bme.readTemperature()*180)/100)+32;
  float alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float pres = bme.readPressure() / 100.0F;
  float hum = bme.readHumidity();
  

  //send data to web server
  if (!client.connected()) {
    sendToServer(temp, alt, pres, hum);
  }

  //write output from web server
  while (client.available()) {
    char c = client.read();
    Serial.write(c);

  }
  
  // if the server's disconnected, stop the client:
//  if (!client.connected()) {
//    Serial.println();
//    Serial.println("disconnecting from server.");
//    client.stop();
//
//    // do nothing forevermore:
//    //while (true);
//  }

    
    
    if(!sel)sel=1;
  
    Serial.print("Temperature = ");
    Serial.print(temp);
    Serial.println(" *F");

    Serial.print("Pressure = ");

    Serial.print(pres);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(alt);
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(hum);
    Serial.println(" %");

    Serial.println();

    if (!digitalRead(BUTTON_A)) sel=1;
    if (!digitalRead(BUTTON_B)) sel=2;
    if (!digitalRead(BUTTON_C)) sel=3;

    display.clearDisplay();
    display.setTextSize(2.5);
    display.setTextColor(WHITE);
    display.setCursor(0,0);

    //decide what val to show on screen
    switch ( sel ) {
    case 3:
      display.print("pres:");
      display.print(bme.readPressure()); //convert to F
      //display.print(sel);
      break;
    case 2:
      display.print("hum:");
      display.print(bme.readHumidity()); //convert to F
      //display.print(sel);
      break;
    case 1:
      display.print("tmp:");
      display.print(temp); //convert to F
      //display.print(sel);
      break;
    default:

      display.print("Tmp:");
      display.print(((bme.readTemperature()*180)/100)+32); //convert to F
      //display.print(sel);

      break;
    }
    
    display.setCursor(0,0);
    display.display(); // actually display all of the above

    delay(10000);
}
