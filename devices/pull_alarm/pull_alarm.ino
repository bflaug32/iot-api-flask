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

const char* APIserver = "http://auwebservice00-env.us-east-1.elasticbeanstalk.com/";  // server's address
const char* APIresource = "/api/v1/getalarm";                // JSON Page



/* Libraries
********************/
#include <ArduinoJson.h>    //https://github.com/bblanchon/ArduinoJson/wiki
#include <WiFi101.h>        //https://www.arduino.cc/en/Reference/WiFi101

/* Arduino Output Pins
********************/
#define LED_PIN     5

//Initialize Hue, Saturation, Brightness values
unsigned int hueVal;
byte satVal = 255;
byte briVal = 255;


/*WiFi Objects and Variables
********************/
#define API_RESPONSE_ARRAY_SIZE  800
int status = WL_IDLE_STATUS;
WiFiClient APIclient;
unsigned long APIcallTimer = 0;  //timer to pace API calls
char APIresponse[API_RESPONSE_ARRAY_SIZE]; //char (byte) array to hold API Response string
unsigned int parsedResponses = 0;
unsigned long APIcallWaitTime = 1500; // in ms; time to wait before next API call

unsigned long ledPinTimer = 0;
unsigned long printTimer = 0;
bool firstRunFlag = true;


/* Setup Routine
********************/
void setup() {
  
      /* Initialize Pin Modes
      ********************/
          pinMode(LED_PIN, OUTPUT);
      
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
            
          } // Exit when Wifi is connected
        

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
                      
                            if(int(APIresponse[0]) != 'O'){   //If the first character is not "123" (ASCII code for open bracket '{')
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
                            }                            } 
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

