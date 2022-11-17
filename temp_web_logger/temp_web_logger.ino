#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include "NTPClient.h"

#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//const char* ssid = "air2bite_wifiE618";
//const char* password = "XAJSG97494";
const char* ssid = "cent-001";
const char* password = "Gd3035303731";
const char* url = "http://www.centemero.com/bertemp.php";


String jsontext;
unsigned long lastTime = 0;
unsigned long timerDelay = 300000;
unsigned long counter = 0;
int sensorId = 1;
// Define NTP Client to get time
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 3600;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


void setup(void)
{
Serial.begin(9600);

WiFiConnectGetIP();
timeClient.begin();
sensors.begin();
delay(6000);
}

void loop(void)
{
  counter++ ;
   
  timeClient.update();
  time_t timestamp = timeClient.getEpochTime();
  timestamp = timestamp + (LegalTime(timestamp) * 60 * 60 ); // aggiungi ora legale
//Serial.println(String("ora legale: ") + LegalTime(timestamp));
  
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempCByIndex(0);
  
  bool timeok = millis() - lastTime >= timerDelay;
  if(tempC >= -10 && tempC <= 50 && timeok)// Check if reading was successful 
  {
  lastTime = millis();
  jsontext = String("[");
  jsontext = jsontext + timestamp + "000";
  jsontext = jsontext + String(",");
  jsontext = jsontext + tempC;
  jsontext = jsontext + String("]");
  sendjson();
  Serial.println(jsontext);
  } 
  else
  {
    int lapsed = (timerDelay - (millis() - lastTime))/1000;
    String msg = String(lapsed) + String(" secs to go - ") + tempC + "°C; loops: " + String(counter);
    Serial.println(msg);
  }
  
  if(counter>=40000){ // should be around 8 hours
    Serial.println("Reset..");
    ESP.restart();
  }
}

int LegalTime(time_t timestamp) {
          int cFlag = 0;
          
struct tm *ptm = gmtime ((time_t *)&timestamp); 

    const int iDayW = ptm->tm_wday;
    const int iDay  = ptm->tm_mday;
    const int iMonth= ptm->tm_mon+1;
    const int iHour = ptm->tm_hour;

//Serial.println(String("WeekDay: ") + iDayW + String(" MonthDay: ") + iDay + String(" Month: ") + iMonth + String(" Hour: ") + iHour);
     
    if (iMonth == 10) {
      if (iDayW == 0) {
          if (((iDay + 7) > 31) && (iHour >= 3)) { cFlag = 0; }
      } else {
        if ((iDay + (7 - iDayW))> 31) { cFlag = 0; }
        else { cFlag = 1; }
      }
    }
     
    if (iMonth == 3) {
      if (iDayW == 0) {
        if (((iDay + 7) > 31) && (iHour >= 2)) { cFlag = 1; }
      } else {
        if((iDay + (7 - iDayW))> 31) { cFlag = 1; } else { cFlag = 0; }
      }
    }
     
    if(iMonth >= 4 && iMonth <= 9) { cFlag = 1; }
    if((iMonth >= 1 && iMonth <= 2) || (iMonth >= 11 && iMonth <= 12)) { cFlag = 0; }
     
    return cFlag;
}

void WiFiConnectGetIP () {
    WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP Address: ");  Serial.println(WiFi.localIP());
}

void sendjson(){
      if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      http.begin(client, url);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsontext);
      Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
      WiFiConnectGetIP();
            if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      http.begin(client, url);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(jsontext);
      Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
      http.end();
    }
    }
}
