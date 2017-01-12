/* User Settings
*********************/ 
const char* ssid = "FiOS-QZ1MU";      //  your network SSID (case sensitive)
const char* pass = "zen84spacey69zoom";   // your network password (case sensitive)
const char* APIserver = "bradflaugher.com";
const char* alarm_resource = "/api/v1/getalarm";
const char* temp_resource = "/api/v1/settemp";


/* Libraries
********************/
#include <WiFi101.h>        //https://www.arduino.cc/en/Reference/WiFi101
#include <SimpleDHT.h>

/* Arduino Output Pins
********************/
#define LAMP_PIN     5
int pinDHT11 = 4;
#define GREEN_PIN    3
#define RED_PIN     2
SimpleDHT11 dht11;



/*WiFi Objects and Variables
********************/
int status = WL_IDLE_STATUS;
WiFiClient client;

void setup() {
    pinMode(LAMP_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    Serial.begin(9600);
    connectWifi();
}


void loop() {
  //GET ALARM
  if(client.connect(APIserver, 80)){  

    client.print("GET ");
    client.print(alarm_resource);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(APIserver);
    client.println("Connection: close");
    client.println();

    delay(500);

    if(client.find("ON")){
        Serial.println("ON");
        digitalWrite(LAMP_PIN, HIGH);
    }
    else{
        Serial.println("OFF");
        digitalWrite(LAMP_PIN, LOW);
    }
        
    client.stop(); 
    delay(60000);
  
  }
  else{
      Serial.println("Reconnecting");
      connectWifi();
  }
  
  //GET HUMIDITY AND TEMP
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  if (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
    Serial.print("Read DHT11 failed.");
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" %");

  char x1[] = "?t=";
  char x2[] = "&h=";
  char combinedArray[sizeof(temp_resource) + sizeof(x1) + sizeof(temperature) + sizeof(x2) + sizeof(humidity)];
  sprintf(combinedArray, "%s%s%d%s%d", temp_resource, x1, (int)temperature, x2, (int)humidity);
  Serial.println(combinedArray);

  if(client.connect(APIserver, 80)){
    client.print("GET ");
    client.print(combinedArray);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(APIserver);
    client.println("Connection: close");
    client.println();

    delay(500);

    if(client.find("OK")){
        Serial.println("ON");
        digitalWrite(GREEN_PIN,HIGH);
        digitalWrite(RED_PIN,LOW);
    }
    else{
        digitalWrite(GREEN_PIN,HIGH);
        digitalWrite(RED_PIN,LOW);
    }

  }

  // DHT11 sampling rate is 1HZ.
  delay(10000);
  
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
