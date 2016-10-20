/*NYCDA & NextFab IoT Workshop
 * Node.js Smart Light
 * by Matt Garfield
 * 8/28/2016
 * Version: MKR1000_NYCDA-API-colors_v1-6_MG
 * 
 * Description:
 * Pull JSON Objects containing "id" and "red", "green", "blue" values from http://node-smartlight.herokuapp.com/api/last_ten
 *    - Change color of LEDs based on latest value of "red", "green", "blue" values
 *    - Display rainbow between color changes
 *    - Display white strobe if there is a JSON parcing error (requires reboot)
*/ 

/* User Settings
*********************/ 
const char* ssid = "NextFab";      //  your network SSID (case sensitive)
const char* pass = "makeithere";   // your network password (case sensitive)

const char* APIserver = "node-smartlight.herokuapp.com";  // server's address
const char* APIresource = "/api/last_ten";                // JSON Page



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
#define RED_INDEX     1
#define GREEN_INDEX   2
#define BLUE_INDEX    3



//Hue Value Color Map from Adafruit
//Used for rainbow()
//https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
#define RED_HUE     0
#define YELLOW_HUE  128
#define GREEN_HUE   256
#define TEAL_HUE    384
#define BLUE_HUE    512
#define PURPLE_HUE  640

//Initialize Hue, Saturation, Brightness values
unsigned int hueVal;
byte satVal = 255;
byte briVal = 255;

//Initialize array to hold RED, GREEN, BLUE color values from hsb2rgb() routine
byte color[3];

//Adjustment factors for RGB LEDs
//Some colors appear stronger in different manufacturerer's LEDs
//These variables let us compensate for these differences
float redAdjust = 1.0;
float greenAdjust = 0.85;
float blueAdjust = 0.85;



/*WiFi Objects and Variables
********************/
#define API_RESPONSE_ARRAY_SIZE  800
int status = WL_IDLE_STATUS;
WiFiClient APIclient;
unsigned long APIcallTimer = 0;  //timer to pace API calls
char APIresponse[API_RESPONSE_ARRAY_SIZE]; //char (byte) array to hold API Response string
unsigned int parsedResponses = 0;
unsigned long APIcallWaitTime = 1500; // in ms; time to wait before next API call



//Current and Previous values to check for updated values
int currentID;              //currentID == idArray[0]
int previousID;             //previousID == idArray[1]
int currentRedVal;          //currentRedVal == redArray[0]
int previousRedVal;         //previousRedVal == redArray[1]
int currentGreenVal;        //currentGreenVal == greenArray[0]
int previousGreenVal;       //previousGreenVal == greenArray[1]
int currentBlueVal;         //currentBlueVal == blueArray[0]
int previousBlueVal;        //previousBlueVal == blueArray[1]

const int sizeOfColorArray  = 10;
int idArray[sizeOfColorArray];
int redArray[sizeOfColorArray];
int greenArray[sizeOfColorArray];
int blueArray[sizeOfColorArray];

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
                digitalWrite(LED_PIN, HIGH);
                breatheHSB (RED_HUE, 255, 128, 5);
                digitalWrite(LED_PIN, LOW);
            }

          } // Exit when Wifi is connected
          
          ledStripOff();  //turn off LED strip

      /* Successful Wifi Connection Indicator Light Sequence
      ********************/ 
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

          ledStripOff();  //turn off LED strip

          printWifiStatus();    // Print Wifi connection info to Serial Monitor

} //END Setup

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
            
            //delay(500);
        
            //Issue request for data from server
            //Uses HTTP commands "printed" as though they were being typed
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

            //Detect end of valid string returned from APIserver
                //Search the returned string for 2 consecutive "new line" sequences ("\r\n\r\n")
                    //if find() is successful, return 'true'
                    //if find() is unsuccessful within setTimeout() ms, return false
            bool validResponse = APIclient.find("\r\n\r\n");

            if(!validResponse){
              Serial.println("API Response Invalid or Timed Out");
            }

            //if APIclient got a valid response, continue with parsing the response string for JSON data
            if(validResponse){
                  
                  //API Data Sucessfully Recieved
                  Serial.println("Valid API Data Received");
                  
                  /* Read received API Response data
                  ********************/
                      APIclient.readBytes(APIresponse, API_RESPONSE_ARRAY_SIZE);
                      Serial.print("APIresponse= ");
                      Serial.println(APIresponse);
              
                      // Ensure the start of the response string is a valid JSON container character
                      // Try to recover if the response string begins with junk characters
                            // Search for '{' (ASCII = 123) if response will be JSON Object
                            // Search for '[' (ASCII = 91) if response will be JSON Array (or Array of Objects)
                      
                            if(int(APIresponse[0]) != 123){   //If the first character is not "123" (ASCII code for open bracket '{')
                                Serial.println("Wrong start char detected");
                                int index = 0;
                                bool beginFound = false;
                                while(!beginFound){       
                                    if(int(APIresponse[index]) == 123){   // Step through the string until '{' is found
                                        beginFound = true;
                                        Serial.print("'{' found at ");
                                        Serial.println(index);
                                    }
                                    index++;
                                }   //When '{' is found, "index" will be it's position in the string
                            
                                int eol = sizeof(APIresponse);
                                Serial.print("Length = ");
                                Serial.println(eol);
                                
                                //restructure by shifting the string by "index" positions so '{' is in the [0] position
                                Serial.println("Restructure API Response...");
                                for(int c=0; c<(eol-index); c++){
                                    APIresponse[c] = APIresponse[((c+index)-1)];
                                    Serial.print(APIresponse[c]);
                                }
                            
                                Serial.println("Restructuring Complete.");      
                            }
      
                      //Dedicated function to handle JSON per best practices here:
                      //https://github.com/bblanchon/ArduinoJson/wiki/FAQ#i-found-a-memory-leak-in-the-library
                      
                          bool jsonParsed = handleJson(APIresponse);
      
                      if(jsonParsed){

                        previousRedVal = currentRedVal;
                        previousGreenVal = currentGreenVal;
                        previousBlueVal = currentBlueVal;
                        previousID = currentID;
                        
                        currentRedVal = redArray[0];
                        currentGreenVal = greenArray[0];
                        currentBlueVal = blueArray[0];
                        currentID = idArray[0];
                      }
      
                      else{
                        currentID = -1;      //Trouble code
                        errorState();       // Flash whiteStrobe() to indicate error
                      }
      
                  // Process id and color values from JSON to choose LED action
                  //********************
                      if (currentID != previousID || firstRunFlag){  //run if the id has updated or this is the first run
  
                          if(!firstRunFlag){    //skip rainbow() on first run              
                            rainbow();          //rainbow sequence indicates update to color (new id detected)
                          }

                          //If all 3 values are -1, go into special "Rainbow" mode loop
                          if(currentRedVal == -1 && currentGreenVal == -1 && currentBlueVal ==-1){
                            rainbow();
                          }
                          else{
                         
                            //Display Color intensities on LEDs
                            analogWrite(RED_PIN, currentRedVal*redAdjust);
                            analogWrite(GREEN_PIN, currentGreenVal*greenAdjust);
                            analogWrite(BLUE_PIN, currentBlueVal*blueAdjust);
                            
                          }
                      }
                   
                      firstRunFlag = false;     //turn off first run flag
 
            } //End of API Response Processing
                
            APIclient.stop(); // disconnect from server      
          
          } //End of API Host Server Connection
    
    Serial.print("** Uptime: ");
    Serial.print(millis()/(1000*60*60*24));
    Serial.print(" days, ");
    Serial.print((millis()/(1000*60*60))%24);
    Serial.print(" hrs, ");
    Serial.print((millis()/(1000*60))%60);
    Serial.print(" mins, ");
    Serial.print((millis()/(1000))%60);
    Serial.println(" secs");
    Serial.println("");       //print blank line to space Serial Monitor read out
  
} //END Loop

/* Subroutines
********************/

/***************************************************************************
handleJson()
  * Parse JSON and set values to variables
  * Use methods detailed here: https://github.com/bblanchon/ArduinoJson/wiki/API-Reference
  * 
  *  Example JSON Object:
  *  ----------------------
     {  "status": "success",
        "colors": 
        [ { "id": 2,
            "red": 39,
            "green": 24,
            "blue": 24
          },
          { "id": 1,
            "red": 100,
            "green": 100,
            "blue": 100
          }
        ]
      }
*****************************************************************************/
bool handleJson(char charArray[]){
  
    bool jsonParseStatus;
  
    // Parse API response into JSON Array
    //********************
        DynamicJsonBuffer jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(APIresponse);   //create root Object

        //if parsing fails...
        if (!root.success()) {  
            jsonParseStatus = false;
            Serial.println("JSON parsing failed");
            Serial.println("");
        }

        //if parsing succeeds...
        else {                  
            parsedResponses++;
            jsonParseStatus = true;
            Serial.println("JSON parsing successful");
            Serial.print(parsedResponses);
            Serial.println(" parsed API responses");

            // Set JSON values to variables
            // Where the MAGIC happens!
            //********************
            JsonArray& colors = root["colors"];   //set nested array object to separate JSON array object for parsing
            
              // step through each set in the color JSON Object
              for(int i=0; i < sizeOfColorArray; i++){
                  idArray[i] = colors[i]["id"];
                  redArray[i] = colors[i]["red"];
                  greenArray[i] = colors[i]["green"];
                  blueArray[i] = colors[i]["blue"];
                  Serial.print("i= ");
                  Serial.print(i);
                  Serial.print(", id: ");
                  Serial.print(idArray[i]);
                  Serial.print(", R: ");
                  Serial.print(redArray[i]*redAdjust);
                  Serial.print(", G: ");
                  Serial.print(greenArray[i]*greenAdjust);
                  Serial.print(", B: ");
                  Serial.println(blueArray[i]*blueAdjust);
              }

        }
                
    return jsonParseStatus;

} //END handleJsonArray()


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

} //END printWifiStatus()

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
    
      color[RED_INDEX]  = (uint8_t)r_temp;
      color[GREEN_INDEX]  = (uint8_t)g_temp;
      color[BLUE_INDEX] = (uint8_t)b_temp;

} //END hsb2rgb()

/***************************************************************************
breatheHSB(int hue, byte saturation, byte brightness, int wait)
  * Runs a single fade-on / fade-out sequence
  * Requires hsbrgb()
  * 
  * hue = color to fade into; 0: full red, 256: full green, 512: full blue, 767: full red
  * saturation = intensity of hue component; 0 = full white, 255 = full hue
  * brightness = intensity of output light; 0 = off, 255 = full brightness
  * wait = number of milliseconds to wait between loops
*****************************************************************************/
void breatheHSB (int h, byte s, byte b, int wait){
      
      for(int i = 0; i <= b; i++){      
          hsb2rgb(h, 255, i, color);
      
          //Set PWM values to Color Pins
          analogWrite(RED_PIN, color[RED_INDEX]);
          analogWrite(GREEN_PIN, color[GREEN_INDEX]);
          analogWrite(BLUE_PIN, color[BLUE_INDEX]);
    
          delay(wait);    //sets speed of fade
      }

      for(int i = b; i >= 0; i--){      
          hsb2rgb(h, 255, i, color);
      
          //Set PWM values to Color Pins
          analogWrite(RED_PIN, color[RED_INDEX]);
          analogWrite(GREEN_PIN, color[GREEN_INDEX]);
          analogWrite(BLUE_PIN, color[BLUE_INDEX]);
    
          delay(wait);    //sets speed of fade
      }
  
} //END breatheHSB()

/***************************************************************************
rainbow()
  * Display sequence of rainbow colors on LEDs
  * Requires hsbrgb(), ledStripOff()
*****************************************************************************/
void rainbow(){
      for(int j = 0; j < 768; j++){      
          hsb2rgb(j, 255, 255, color);
      
          //Set PWM values to Color Pins
          analogWrite(RED_PIN, color[RED_INDEX]);
          analogWrite(GREEN_PIN, color[GREEN_INDEX]);
          analogWrite(BLUE_PIN, color[BLUE_INDEX]);
    
          delay(2);
      }
      
  ledStripOff();  //turn LEDs Off
  
} //END rainbow()

/***************************************************************************
ledStripOff()
  * turn off LED strip
*****************************************************************************/
void ledStripOff(){
  //turn Off LED Strip
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

/***************************************************************************
errorState()
  * Display LED sequence to indicate to the user that there has been an error
  * Requires reboot
*****************************************************************************/
void errorState(){
  
  while(1){
    whiteStrobe();
  }

}

