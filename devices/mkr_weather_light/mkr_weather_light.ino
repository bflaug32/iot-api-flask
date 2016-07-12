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

unsigned long APIcallWaitTime = 2000; // in ms; time to wait before next API call

/* Libraries
********************/
#include <ArduinoJson.h>    //https://github.com/bblanchon/ArduinoJson/wiki
#include <WiFi101.h>        //https://www.arduino.cc/en/Reference/WiFi101

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

#define API_RESPONSE_ARRAY_SIZE  400

/*WiFi Objects and Variables
********************/
int status = WL_IDLE_STATUS;
WiFiClient APIclient;
unsigned long APIcallTimer = 0;  //timer to pace API calls
char APIresponse[API_RESPONSE_ARRAY_SIZE]; //char (byte) array to hold API Response string
unsigned int parsedResponses = 0;

//Initialize Hue, Saturation, Brightness values
unsigned int hueVal;
byte satVal = 255;
byte briVal = 255;

//Initialize array to hold RED, GREEN, BLUE color values from hsb2rgb() routine
byte color[3]; 

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


} 
//End Setup

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

            //API Data Sucessfully Recieved
            Serial.println("API Request Sent");

            //Set Time limit for API.client to wait for find() to be successful
            APIclient.setTimeout(APIcallWaitTime);

            //if find() is successful, return 'true'
            //if find() is unsuccessful within setTimeout() ms, return false
            bool validResponse = APIclient.find("\r\n\r\n");

            if(!validResponse){
              Serial.println("API Response Invalid or Timed Out");
            }

            //if APIclient got a valid response, continue with parsing response
            if(validResponse){
                  
                  //API Data Sucessfully Recieved
                  Serial.println("Valid API Data Received");
                  
                  /* Read received API Response data
                  ********************/
                      APIclient.readBytes(APIresponse, API_RESPONSE_ARRAY_SIZE);
                      Serial.print("APIresponse= ");
                      Serial.println(APIresponse);
              
                      // make sure the response stream header is valid
                      // Parsing fails if this data is not removed
                      
                      if(int(APIresponse[0]) != 91){   //"91" is ASCII code for open bracket "["
                          Serial.println("Wrong start char detected");
                          int index = 0;
                          bool beginFound = false;
                          while(!beginFound){
                              if(int(APIresponse[index]) == 91){ // check for the "[" 
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
      
                      //Dedicated function to handle JSON per best practices here:
                      //https://github.com/bblanchon/ArduinoJson/wiki/FAQ#i-found-a-memory-leak-in-the-library
                      bool jsonParsed = handleJsonArray(APIresponse);
      
                      if(jsonParsed){
                        thisState = stateArray[9];
                        thisID = idArray[9];
                      }
      
                      else{
                        thisState = -1;   //Trouble code
                        thisID = -1;      //Trouble code
                      }
      
                  /* Process id and state values from JSON array to choose LED reaction
                  ********************/
                      if (thisID != lastID || firstRunFlag){  //run if the id has updated or this is the first run
      
                          int lastHueVal = hueVal;  //save hue value from last run
      
                          /* Compare last 5 states, if they match display rainbow()
                          ********************/
                              boolean matchLast5 = true;
                              for(int i = 9; i > 5; i--){
                                int test1 = stateArray[i];
                                int test2 = stateArray[i-1];
                                if(test1 != test2){ matchLast5 = false; }
                              }
                              
                              if(matchLast5 == true){ Serial.println("-- Rainbow!"); rainbow(); }
              
                          /* If all 10 states == 0, display whiteStrobe()
                          ********************/
                              boolean lastTen0 = true;
                              for(int i = 9; i > 0; i--){
                                int test1 = stateArray[i];
                                if(test1 != 0){ lastTen0 = false; }
                              }
                              
                              if(lastTen0 == true){ Serial.println("-- White Strobe!"); whiteStrobe(); }
      
                          /* Update LED Colors based on latest state
                          ********************/
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
                                default:  //if thisState does not match any case, indicate trouble display 
                                    hueVal = 512;   //Blue - trouble code
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
                   
                      firstRunFlag = false;     //turn off first run flag
 
            }
            //End of API Response Processing
                
            APIclient.stop(); // disconnect from server      
          } 
          //End of API Host Server Connection
    
    Serial.print("** Uptime= ");
    Serial.print(millis()/(1000*60*60*12));
    Serial.print(" days, ");
    Serial.print((millis()/(1000*60*60))%24);
    Serial.print(" hrs, ");
    Serial.print((millis()/(1000*60))%60);
    Serial.print(" mins, ");
    Serial.print((millis()/(1000))%60);
    Serial.println(" secs");
    Serial.println("");       //print blank line to space Serial Monitor read out
          
  
} 
//End of Loop

/* Subroutines
********************/

bool handleJsonArray(char charArray[]){
  
    bool jsonParseStatus;
  
    /* Parse API response into JSON Array
    ********************/
        DynamicJsonBuffer jsonBuffer;
        JsonArray& root = jsonBuffer.parseArray(APIresponse);
        
        if (!root.success()) {
            jsonParseStatus = false;
            Serial.println("JSON parsing failed");
            Serial.println("");
        }
        
        else {
            parsedResponses++;
            jsonParseStatus = true;
            Serial.println("JSON parsing successful");
            Serial.print(parsedResponses);
            Serial.println(" parsed API responses");
            
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
              }
        }
                
        return jsonParseStatus;
}


/***************************************************************************
printWifiStatus()
  * Display WiFi connection information on Serial Monitor
*****************************************************************************/
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

/***************************************************************************
rainbow()
  * Display sequence of rainbow colors on LEDs
*****************************************************************************/
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

/***************************************************************************
whiteStrobe()
  * Display sequence of white light flashes on LEDs
*****************************************************************************/
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

