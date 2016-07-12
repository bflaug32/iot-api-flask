/*NYCDA & NextFab IoT Workshop
 * Web Connected Smart Light
 * by Matt Garfield
 * 
 * Description:
 * Pull JSON Array of 10 JSON Objects containing "id" and "state" values from http://nycda-nextfab.herokuapp.com/datas 
 *    - Change color of LEDs based on latest value of "state"
 *    - Display rainbow if last 5 states match
 *    - Display white strobe if state = 0 ten times in a row
*/ 

/* User Settings
*********************/ 
const char* ssid = "NextFab";      //  your network SSID (case sensitive)
const char* pass = "makeithere";   // your network password (case sensitive)

const char* APIserver = "nycda-nextfab.herokuapp.com";  // server's address
const char* APIresource = "/datas";

unsigned long APIcallWaitTime = 1000; // in ms; time to wait before next API call

/* Libraries
********************/
#include <ArduinoJson.h>
#include <WiFi101.h>

/*Declare JSON Buffer Object
********************/
DynamicJsonBuffer jsonBuffer;
#define BUFFER_SIZE  600
//StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

/*WiFi Objects and Variables
********************/
int status = WL_IDLE_STATUS;
WiFiServer MKRserver(80);
WiFiClient APIclient;
unsigned long APIcallTimer = 0;  //timer to pace API calls
char APIresponse[BUFFER_SIZE]; 
int parsedResponses = 0;

/* Arduino Output Pins
********************/
#define LED_PIN     6
#define RED_PIN     5
#define GREEN_PIN   4
#define BLUE_PIN    3

//Use color names as index values for color[]
#define RED     1
#define GREEN   2
#define BLUE    3

//Hue Value Color Map from Adafruit
//https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
#define RED_VAL   0
#define BLUE_VAL  512

//Initialize Hue, Saturation, Brightness values
uint16_t hueVal;
uint8_t satVal = 255;
uint8_t briVal = 255;

//Initialize array to hold RED, GREEN, BLUE color values from hsb2rgb() routine
uint8_t color[3]; 

int thisState;  //thisState == stateArray[9]
int lastState;  //lastState == stateArray[8]
int thisID;     //thisID == idArray[9]
int lastID;     //lastID == idArray[8]
int idArray[10];
int stateArray[10];

unsigned long ledPinTimer = 0;
unsigned long printTimer = 0;
bool firstRunFlag = true;


/* Setup Routine
********************/
void setup() {
  
      /* Initialize Pin Modes
      ********************/
          pinMode(LED_PIN, OUTPUT);
          pinMode(RED_PIN, OUTPUT);
          pinMode(BLUE_PIN, OUTPUT);
          pinMode(GREEN_PIN, OUTPUT);
      
      /* Initialize Serial communication
      ********************/
          Serial.begin(9600);
          Serial.print("Start Serial ");

      /* Check for the presence of Wifi Hardware
      ********************/ 
          Serial.print("WiFi101 hardware: ");
          
          if (WiFi.status() == WL_NO_SHIELD) {
            Serial.println("NOT DETECTED");
            return; // don't continue
          }
          
          Serial.println("DETECTED");

      
      /* Attempt to connect to Wifi network:
      ********************/
          while ( status != WL_CONNECTED) {
            
            Serial.print("Attempting to connect to SSID: ");
            Serial.println(ssid);   // print the network name (SSID)
            
            // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
            status = WiFi.begin(ssid, pass);
            
            // Wait 8 seconds for connection attempt
            uint16_t wifiConnectTimer = millis();   //set timer
            
            while(millis() - wifiConnectTimer < 8000){
              
                //"Breathe" Red LEDs, Flash Onboard LED
                for(int i=0; i<128; i++){
                  analogWrite(RED_PIN, i);
                  digitalWrite(LED_PIN, LOW);
                  delay(5);
                }
                for(int i=128; i > 0; i--){
                  analogWrite(RED_PIN, i);
                  digitalWrite(LED_PIN, HIGH);
                  delay(5);
                }
            }
            
          // Exit when Wifi is connected
          }


      /* Successful Wifi Connection Indicator Light Sequence
      ********************/
          //turn off Red LEDs
          analogWrite(RED_PIN, 0);  
          
          //Fade Up Green LEDs
          for(int i=0; i<128; i++){
                analogWrite(GREEN_PIN, i);
                delay(5);
          }
          
          //Flash Onboard LED
          ledPinTimer = millis();
          while(millis() - ledPinTimer < 1000){
            digitalWrite(LED_PIN, LOW);
            delay(50);
            digitalWrite(LED_PIN, HIGH);
            delay(50);  
          }

          //turn off Green LEDs
          analogWrite(GREEN_PIN, 0);  

      printWifiStatus();    // Print Wifi connection info to Serial Monitor

//End Setup
}


/* Loop
********************/
void loop() {

      /* Attempt to retrieve data from API Host Server
       *  MKR1000   = client
       *  API Host  = server
      ********************/
          //if client successfully connects to server AND more than APIcallWaitTime seconds have passed OR it's the first run
          if( APIclient.connect(APIserver, 80) && (( (millis() - APIcallTimer ) > APIcallWaitTime ) || firstRunFlag)){
            
            
            APIcallTimer = millis();  //reset APIcallTimer
            
            //Serial.print("APIcallTimer = ");
            //Serial.println(APIcallTimer);
            
            //Flash Onboard LED indicator
            ledPinTimer = millis();
            while(millis() - ledPinTimer < 500){
              digitalWrite(LED_PIN, LOW);
              delay(50);
              digitalWrite(LED_PIN, HIGH);
              delay(50);  
            }
            
            delay(500);
        
            //Send request to resource
            APIclient.print("GET ");
            APIclient.print(APIresource);
            APIclient.println(" HTTP/1.1");
            APIclient.print("Host: ");
            APIclient.println(APIserver);
            APIclient.println("Connection: close");
            APIclient.println();
        
            delay(100);

            //Reading stream and remove headers
            APIclient.setTimeout(1000);
        
            bool ok_header = ok_header = APIclient.find("\r\n\r\n");
        
            while(!ok_header){
            // wait
            }
  
            //API Data Sucessfully Recieved
            Serial.println("API Data Received");
            
        
            /* Read received API Response data
            ********************/
                APIclient.readBytes(APIresponse, BUFFER_SIZE);
        
                // make sure the response stream header is valid
                // Sometime OWM includes invalid data after the header
                // Parsing fails if this data is not removed
                
                if(int(APIresponse[0]) != 91){   //"91" is ASCII code for open bracket "["
                    Serial.println("Wrong start char detected");
                    int index = 0;
                    bool beginFound = false;
                    while(!beginFound){
                        if(int(APIresponse[index]) == 91){ // check for the "{" 
                        beginFound = true;
                        Serial.print("'[' found at ");
                        Serial.println(index);
                        }
                        index++;
                    }
                
                    int eol = sizeof(APIresponse);
                    Serial.print("Length = ");
                    Serial.println(eol);
                    
                    //restructure by shifting the correct data
                    Serial.println("Restructure API Response...");
                    for(int c=0; c<(eol-index); c++){
                        APIresponse[c] = APIresponse[((c+index)-1)];
                        Serial.print(APIresponse[c]);
                    }
                
                    Serial.println("Restructuring Complete.");      
                }
                
        
            /* Parse API response into JSON Object
            ********************/
                //JsonObject& root = jsonBuffer.parseObject(APIresponse);
                //JsonArray& root = jsonBuffer.createArray();
                JsonArray& root = jsonBuffer.parseArray(APIresponse);

                root.prettyPrintTo(Serial);
                Serial.println("");
                
                if (!root.success()) {
                    Serial.println("JSON parsing failed!");
                    Serial.println("");
                    thisState = -1;   //Trouble code
                    thisID = -1;      //Trouble code
                } 
                
                else {
                    parsedResponses++;
                    Serial.println("JSON parsing worked!");
                    Serial.print(parsedResponses);
                    Serial.println(" successfully parsed responses!");
                    Serial.println("");
                    
                    /* Set JSON values to variables
                    ********************/
                      for(int i=0; i<10; i++){
                          idArray[i] = root[i]["id"];
                          stateArray[i] = root[i]["state"];
                          Serial.print("i= ");
                          Serial.print(i);
                          Serial.print(", id: ");
                          Serial.print(idArray[i]);
                          Serial.print(", state: ");
                          Serial.println(stateArray[i]);
    
                          thisState = stateArray[9];
                          thisID = idArray[9];
                      }
                }

            /* Update LED Colors based on latest state
            ********************/
               
                if (thisID != lastID || firstRunFlag){

                    int lastHueVal = hueVal;

                    /* Compare last 5 states, if they match display rainbow()
                    ********************/
                        boolean matchLast5 = true;
                        for(int i = 9; i > 5; i--){
                          int test1 = stateArray[i];
                          int test2 = stateArray[i-1];
                          if(test1 != test2){ matchLast5 = false; }
                        }
                        
                        if(matchLast5 == true){ rainbow();}
        
                    /* If all 10 states == 0, display whiteStrobe()
                    ********************/
                        boolean lastTen0 = true;
                        
                        for(int i = 9; i > 0; i--){
                          int test1 = stateArray[i];
                          
                          if(test1 != 0){ lastTen0 = false; }
                          
                        }
                        
                        if(lastTen0 == true){ whiteStrobe();}
    
                    switch (thisState) {
                          case 0:
                              hueVal = 256; //Green
                            break;
                          case 1:
                              hueVal = 100; //Yellow
                            break;
                          case 2:
                              hueVal = 0;   //Red
                          break;
                          default: 
                              hueVal = 512;   //Blue
                          break;
                        }

                    if(!firstRunFlag){    //skip fade out on first run

                        //Fade Out Old LED Color
                        for(int i = briVal; i > 0; i--){ 
                            //Convert Hue value to RGB values
                            //returns array color[RED, GREEN, BLUE]
                            hsb2rgb(lastHueVal, satVal, i, color);
                     
                            //Display Color intensities on LEDs
                            analogWrite(RED_PIN, color[RED]);
                            analogWrite(GREEN_PIN, color[GREEN]);
                            analogWrite(BLUE_PIN, color[BLUE]);
                            delay(2);
                        }
                    }

                    //Fade In New LED Color
                    for(int i = 0; i < briVal; i++){ 
                        //Convert Hue value to RGB values
                        //returns array color[RED, GREEN, BLUE]
                        hsb2rgb(hueVal, satVal, i, color);
                 
                        //Display Color intensities on LEDs
                        analogWrite(RED_PIN, color[RED]);
                        analogWrite(GREEN_PIN, color[GREEN]);
                        analogWrite(BLUE_PIN, color[BLUE]);
                        delay(2);
                    }
                    

                    lastState = thisState;  //update lastState
                    lastID = thisID;        //update lastID
                }
             
                APIclient.stop(); // disconnect from server
                
                Serial.println("");
                
                firstRunFlag = false;     //turn off first run flag
          
          //End of API Host Server Routine
          }
          
//End of Loop  
}


/* Subroutines
********************/

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  //Serial.print("To see this page in action, open a browser to http://");
  //Serial.println(ip);
}

/******************************************************************************
 hsb2rgb()
 Convert Hue, Saturation, Brightness to Red, Green, Blue values
 
 https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
 
 * accepts hue, saturation and brightness values and outputs three 8-bit color
 * values in an array (color[])
 *
 * saturation (sat) and brightness (bright) are 8-bit values.
 *
 * hue (index) is a value between 0 and 767. hue values out of range are
 * rendered as 0.
 *
 *****************************************************************************/
void hsb2rgb(uint16_t index, uint8_t sat, uint8_t bright, uint8_t color[3]){

      uint16_t r_temp, g_temp, b_temp;
      uint8_t index_mod;
      uint8_t inverse_sat = (sat ^ 255);
    
      index = index % 768;
      index_mod = index % 256;
    
      if (index < 256)
      {
        r_temp = index_mod ^ 255;
        g_temp = index_mod;
        b_temp = 0;
      }
    
      else if (index < 512)
      {
        r_temp = 0;
        g_temp = index_mod ^ 255;
        b_temp = index_mod;
      }
    
      else if ( index < 768)
      {
        r_temp = index_mod;
        g_temp = 0;
        b_temp = index_mod ^ 255;
      }
    
      else
      {
        r_temp = 0;
        g_temp = 0;
        b_temp = 0;
      }
    
      r_temp = ((r_temp * sat) / 255) + inverse_sat;
      g_temp = ((g_temp * sat) / 255) + inverse_sat;
      b_temp = ((b_temp * sat) / 255) + inverse_sat;
    
      r_temp = (r_temp * bright) / 255;
      g_temp = (g_temp * bright) / 255;
      b_temp = (b_temp * bright) / 255;
    
      color[RED]  = (uint8_t)r_temp;
      color[GREEN]  = (uint8_t)g_temp;
      color[BLUE] = (uint8_t)b_temp;
}

void rainbow(){
  
  for(int i = 0; i < 3; i++){
      for(int j = 0; j < 768; j++){      
          hsb2rgb(j, 255, 255, color);
      
          //Set PWM values to Color Pins
          analogWrite(RED_PIN, color[RED]);
          analogWrite(GREEN_PIN, color[GREEN]);
          analogWrite(BLUE_PIN, color[BLUE]);
    
          delay(2);
    
      }
  }
  
  //turn LEDs Off
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);
  
}

void whiteStrobe(){

    for(int i=0; i<10; i++){
      
          //turn LEDs White
          analogWrite(RED_PIN, 255);
          analogWrite(GREEN_PIN, 255);
          analogWrite(BLUE_PIN, 255);

          delay(50);
        
          //turn LEDs Off
          analogWrite(RED_PIN, 0);
          analogWrite(GREEN_PIN, 0);
          analogWrite(BLUE_PIN, 0);

          delay(50);
    }
}

