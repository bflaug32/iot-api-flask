/* User Settings
*********************/ 
const char* ssid = "NETWORKNAME";      //  your network SSID (case sensitive)
const char* pass = "PASSWORD";   // your network password (case sensitive)
const char* APIserver = "XXXXX.us-east-1.elasticbeanstalk.com";
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

    client.print("GET ");
    client.print(APIresource);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(APIserver);
    client.println("Connection: close");
    client.println();

    delay(500);

    if(client.find("ON")){
        Serial.println("ON");
        digitalWrite(LED_PIN, HIGH);
    }
    else{
        Serial.println("OFF");
        digitalWrite(LED_PIN, LOW);
    }
        
    client.stop(); 
    delay(60000);
  
  }
  else{
      Serial.println("RECONNECTING");
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
