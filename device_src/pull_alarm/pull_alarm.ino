/* User Settings
*********************/ 
const char* ssid = "NextFab";      //  your network SSID (case sensitive)
const char* pass = "makeithere";   // your network password (case sensitive)
const char* APIserver = "http://auwebservice00-env.us-east-1.elasticbeanstalk.com";
const char* APIresource = "/api/v1/getalarm";


/* Libraries
********************/
#include <WiFi101.h>        //https://www.arduino.cc/en/Reference/WiFi101

/* Arduino Output Pins
********************/
#define LED_PIN     5

/*WiFi Objects and Variables
********************/
int status = WL_IDLE_STATUS;
WiFiClient client;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);
    while(!Serial){} //wait for serial connection 
    connectWifi();
}


void loop() {
  if(client.connect(APIserver, 80)){  

    APIclient.print("GET ");
    APIclient.print(APIresource);
    APIclient.println(" HTTP/1.1");
    APIclient.print("Host: ");
    APIclient.println(APIserver);
    APIclient.println("Connection: close");
    APIclient.println();

    delay(500);

    if(APIclient.find("ON"){
        APIclient.readBytes(APIresponse, API_RESPONSE_ARRAY_SIZE);
        digitalWrite(LED_PIN, HIGH);
    }
        
    APIclient.stop(); 
    delay(6000);
  
  }
  else{
      connectWifi();
  }
  
}

void connectWifi() {
    Serial.print("Connecting to WiFi");
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
}
