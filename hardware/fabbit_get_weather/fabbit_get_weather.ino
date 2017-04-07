/*  Robo-Plan Technologies LTD: 08/2016

   The purpose of this Arduino Sketch is to check the ESP8266-01 as a peripheral wifi board connected to
   an Arduino Uno board via a 5-3.3 logic converter with SoftwareSerial using ESP8266 AT command library created by ITEAD.
   
   The original library can be found: https://github.com/itead/ITEADLIB_Arduino_WeeESP8266

   Our goal was to create an Esp8266 AT command library (based on ITEAD library), 
   that would work well on software serial on most ESP8266 devices, no matter the firmware, or the initial baudrate.
   
   Therefore, we are distributing this preliminary library. Please connect the Esp8266-01 and run this example.
   Write to us about any bugs, comments, issues, improvement proposals, etc.

   We have modified the original library due to Software serial baudrate problems.
   Now, the initialize function, when using software serial only, will set the ESP8266 baudrate to 9600.
   
   The sketch sets the ESP8266 baudrate to 9600 by default for software serial and to 115200 with hardware serial. 
   Then it connects to your AP, checks the version of the ESP8266, sends a TCP request to google.com and displays the response on the serial monitor.

   Notes:
   -  In order to run the example, first connect the ESP8266 via Software Serial to your arduino board using a logic converter,
        as shown in the wiring figure attached.
   -  Enter your SSID and PASSWORD below.
   -  If using SoftwareSerial make sure the line "#define ESP8266_USE_SOFTWARE_SERIAL" in ESP8266.h is uncommented.


 Troubleshooting:
   -  If you receive partial response from the esp8266 when using software serial - 
      go to C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\SoftwareSerial\src\SoftwareSerial.h
      Change line 42: #define _SS_MAX_RX_BUFF 64 // RX buffer size
      To: #define _SS_MAX_RX_BUFF 256 // RX buffer size
      this will enlarge the software serial buffer.
   -  Sometime setting the baudrate on initialization fails, try resetting the Arduino, it should work fine.
   
*/

const char* SSID     = "NextFab";
const char* PASSWORD = "makeithere";

const char* APIserver = "bradflaugher.com";
const char* APIresource = "/api/v1/gettemp";
const int   port = 80;

#define BAUD    9600
#define PIN_RX  10
#define PIN_TX  11

#include "ESP8266.h"

SoftwareSerial mySerial(PIN_RX, PIN_TX); //SoftwareSerial pins for MEGA/Uno. For other boards see: https://www.arduino.cc/en/Reference/SoftwareSerial

ESP8266 wifi(mySerial); 

void setup(void)
{
  //Start Serial Monitor at any BaudRate
  Serial.begin(BAUD);
  Serial.print("Connecting to Wifi Network: ");
  Serial.println(SSID);

  if (!wifi.init(SSID, PASSWORD))
  {
    Serial.println("Wifi Init failed. Check configuration.");
    while (true) ; // loop eternally
  }
}

void loop(void)
{   
    Serial.print("Sending GET Request to: ");
    Serial.print(APIserver);
    Serial.println(APIresource);

    //Pull Webpage Data
    wifi.httpGet(APIserver, APIresource, port);
    delay(2000);

    //Release Server Connection
    wifi.releaseTCP();
    delay(1000);
}







