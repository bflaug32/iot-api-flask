/*
  LiquidCrystal Library - display() and noDisplay()

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD and uses the
 display() and noDisplay() functions to turn on and off
 the display.

 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 http://www.arduino.cc/en/Tutorial/LiquidCrystalDisplay

 */


/* User Settings
*********************/ 
const char* ssid = "825_carpenter";      //  your network SSID (case sensitive)
const char* pass = "ladylady";   // your network password (case sensitive)
const char* APIserver = "api.fabbit.io";


/* Libraries
********************/
#include <WiFi101.h>        //https://www.arduino.cc/en/Reference/WiFi101
#include <LiquidCrystal.h>

/*WiFi Objects and Variables
********************/
int status = WL_IDLE_STATUS;
int loop_count = 0;
WiFiClient client;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  connectWifi();
}

void loop() {
  if(client.connect(APIserver, 80)){  

    if(loop_count > 100){
      loop_count = 0;
    }

    // MAKE THE REQUEST (different one each time)
    if (loop_count % 4 == 0){
      client.print("GET ");
      client.print("/api/v1/getstockquote?s=SPLV");
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(APIserver);
      client.println("Connection: close");
      client.println();
    }
    else if (loop_count % 4 == 1){    
      client.print("GET ");
      client.print("/api/v1/getstockquote?s=XLK");
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(APIserver);
      client.println("Connection: close");
      client.println();
    }
    else if (loop_count % 4 == 2){    
      client.print("GET ");
      client.print("/api/v1/getforecastzip?q=19147");
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(APIserver);
      client.println("Connection: close");
      client.println();
    }
    else{
      client.print("GET ");
      client.print("/api/v1/getnflstandings?t=philadelphia");
      client.println(" HTTP/1.1");
      client.print("Host: ");
      client.println(APIserver);
      client.println("Connection: close");
      client.println();
    }

    loop_count += 1;

    delay(500);

    // GET THE RESPONSE
    int count = 0;

    char chararr1[16];
    char chararr2[16];
    boolean started=false;
    
    while(client.available()){        
      char c = client.read();      
      Serial.write(c);

      if (c == '{'){
        started=true;  
      }

      if (started && c != '{' && c != '}' && c != ':' && c != '"'){
        if (count < 16){
          chararr1[count] = c;
        }
        else if (count < 32){
          chararr2[count-16] = c;
        }
        count = count + 1;
      }

    }
    
    while (count < 32){
        if (count < 16){
          chararr1[count] = ' ';
        }
        else if (count < 32){
          chararr2[count-16] = ' ';
        }
        count = count + 1;
    }
    
    Serial.println(chararr1);
    Serial.println(chararr2);
    
    // Turn off the display:
    lcd.noDisplay();
    delay(500);
    lcd.setCursor(0, 0);
    lcd.print(chararr1);
    lcd.setCursor(0, 1);
    lcd.print(chararr2);     
    lcd.display();
        
    client.stop(); 
    delay(6000);

  }
  else{
      Serial.println("Reconnecting");
      connectWifi();
  }
}

void connectWifi() {
    Serial.print("Connecting to WiFi");
    lcd.noDisplay();
    lcd.setCursor(0, 0);
    lcd.print("CONNECTING...");
    lcd.display();
    while ( status != WL_CONNECTED) {
        status = WiFi.begin(ssid, pass);
        Serial.print(".");
        delay(1000); 
    }

    Serial.println("");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
    lcd.noDisplay();
    lcd.setCursor(0, 0);
    lcd.print("CONNECTED AT:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());   
    lcd.display();
    delay(5000);

}
