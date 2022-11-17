#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
  
#include <SoftwareSerial.h>
#define rxPin D5
#define txPin D6
String appendString = "";
boolean nextnetscsan = true;



const char* ssid     = "cent-001";
const char* password = "Gd3035303731";
const char* serverName = "http://www.centemero.com/gps.php";

SoftwareSerial mySerial(rxPin,txPin);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Initializing...");
  
WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
 mySerial.println("AT");
}

void loop()   
{
  String wifname;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t* bssid;
  int32_t channel;
  bool hidden;
  int scanResult; 
  char wifis[255];
  String wifiappend = "";
  
  if(nextnetscsan) {
    delay(15000);
    mySerial.println("AT+CNETSCAN=1");
    mySerial.println("AT+CNETSCAN");
    nextnetscsan = false;
  }
  while(mySerial.available()) {
    String incomingString = mySerial.readStringUntil('\n');
    int operatorPos = incomingString.indexOf('Operator');
//    int cnetscsanPos = incomingString.indexOf('CNETSCAN');
    int okPos = incomingString.indexOf('OK');
    if(operatorPos > 0) {
      appendString +=  '\n' + incomingString;
    }
    if(okPos > 0 & appendString.length() > 0) {
      Serial.println(appendString);

scanResult = WiFi.scanNetworks(false, true);
Serial.println();
Serial.print("wifi scanResult: ");
Serial.println(scanResult);
    for (int8_t i = 0; i < scanResult; i++) {
      WiFi.getNetworkInfo(i, wifname, encryptionType, rssi, bssid, channel, hidden);
      sprintf(wifis, "{\"macAddress\": \"%02X:%02X:%02X:%02X:%02X:%02X\", \"signalStrength\": %d, \"signalToNoiseRatio\": 0},\n",bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], rssi);  
      wifiappend += wifis;
      yield();
    }
        
      if(WiFi.status()== WL_CONNECTED){
     wifiappend  = "\"cellTowers\": [\n" + appendString + "\n],\n" + "\"wifiAccessPoints\": [\n" + wifiappend + "\n]";
     Serial.println(wifiappend);
        WiFiClient client;
        HTTPClient http;
        http.begin(client, serverName); // Your Domain name with URL path or IP address with path
        http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Specify content-type header
        int httpResponseCode = http.POST(wifiappend);     // Send HTTP POST request
        http.end();
        if (httpResponseCode>0) {
          Serial.print("Success code: ");
          Serial.println(httpResponseCode);
         } else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
         }
        }
      appendString = "";
      nextnetscsan = true; 
      delay(10000);
    }
  }
}
