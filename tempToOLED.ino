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


#if (SSD1306_LCDHEIGHT != 32)
 #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int sel;

void setup() {

  
  
//  Serial.begin(9600);
//  Serial.println(F("BME280 and OLED test"));


  if (!bme.begin()) {
//    Serial.println("Could not find a valid BME280 sensor, check wiring!");
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
  delay(1000);
 
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  
}

void loop() {

    
    if(!sel)sel=1;
  
//    Serial.print("Temperature = ");
//    Serial.print(bme.readTemperature());
//    Serial.println(" *C");
//
//    Serial.print("Pressure = ");
//
//    Serial.print(bme.readPressure() / 100.0F);
//    Serial.println(" hPa");
//
//    Serial.print("Approx. Altitude = ");
//    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
//    Serial.println(" m");
//
//    Serial.print("Humidity = ");
//    Serial.print(bme.readHumidity());
//    Serial.println(" %");
//
//    Serial.println();

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
      display.print(((bme.readTemperature()*180)/100)+32); //convert to F
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

    delay(2000);
}
