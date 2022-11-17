// busy lila D1; rst white D2; dc green D3; cs orange D4; gnd brown G;  clk yellow D5; din blue D7; vcc grey 3.3V
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

#include <GxEPD.h>
#include <GxGDEW042T2/GxGDEW042T2.h>      // 4.2" b/w
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>

#include  "icons_60.h";
//cloud2, fog, moon, mooncloud, rain0, rain1, rain2, schirm, snow, suncloud, sunny, thunder0, temp
#include  "icons_30.h";
//cloud2_sml, fog_sml, moon_sml, mooncloud_sml, rain0_sml, rain1_sml, rain2_sml, snow_sml, suncloud_sml, sunny_sml, thunder_sml
#include  "icons_80.h";
//cloud2_b, fog_b, moon_b, mooncloud_b, rain0_b, rain1_b, rain2_b, snow_b, suncloud_b, sunny_b, thunder_b

const char* ssid     = "cent-001";
const char* password = "Gd3035303731";
const String apiurl = "http://www.centemero.com/openweather.json";
const String timeurl = "http://www.centemero.com/time.php";
String payload;
String epochTime;
uint32_t deepsleep;
int tz;
int dttemp ;
unsigned long crntTime;


char* mm[]={ "0", "Jan", "Feb", "Mar", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov","Dec"};
char* dd[]={ "0", "Son", "Mon", "Die", "Mit", "Don", "Fri", "Sam"};
int16_t tbx, tby; uint16_t tbw, tbh;


ESP8266WiFiMulti WiFiMulti;

GxIO_Class io(SPI, D4, D3, D2);
GxEPD_Class display(io, D2, D1); 

void setup()
{
  Serial.begin(115200);
  delay(500);
  Serial.println("Hello!");

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  //WiFiMulti.addAP("Giovannis iPhone", "merocente");


display.init(115200); // enable diagnostic output on Serial
Serial.println("screen intiatied");
display.setRotation(0);

 
while(payload.length()==0){
getinfo();
}

weather();
display.update();
display.powerDown();

Serial.print(" Sleep time: ");
if (dttemp == 150) {
  deepsleep = 60*60*1e6;
  Serial.print(dttemp);
  Serial.println(" - 70 minutes");
}
 else {
  deepsleep = 60*60*1e6;
  Serial.print(dttemp);
  Serial.println(" - 60 minutes");
 }

deepsleep = 2*60*1e6;
ESP.deepSleep(deepsleep); //deep sleep 2 minutes
}

void loop()
{
}

void align(int x, int y, String testo){
  display.getTextBounds(testo, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(x-tbx, y);
}

void corona(int crnty, long dt, long confirmed, String country) {
  //////// ePaper   //weekday, date, hour, temp, id
  display.fillScreen(GxEPD_WHITE);
   
  display.setTextColor(GxEPD_BLACK);  
  display.setFont(&FreeMonoBold9pt7b);
  
  align(230, crnty+36, String(day(dt)) + "." + month(dt) + "." + year(dt));
         display.print(String(day(dt)) + "." + month(dt) + "." + year(dt));
  align(230, crnty+56, String("covid: ") + confirmed);
         display.print(String("covid: ") + confirmed);
}

void current(int crnty, long dt, double temp, int id) {
  //////// ePaper   //weekday, date, hour, temp, id 
  display.drawFastVLine(210, crnty-27, 90, GxEPD_BLACK);
  display.drawFastHLine(100  ,200, 200, GxEPD_BLACK);
  
  display.setFont(&FreeMonoBold18pt7b);
  align(20, crnty, dd[weekday(dt)]);
  display.print(dd[weekday(dt)]); 
  
  display.setFont(&FreeMonoBold12pt7b);
  align(10, crnty+24, String(day(dt)) + " " + mm[month(dt)]);
  display.print(String(day(dt)) + " " + mm[month(dt)]);
  
  display.setFont(&FreeMonoBold18pt7b);
  align(2, crnty+56, String(hour(dt)) + ":" + ((minute(dt) > 9) ? "" : "0")+ minute(dt));
  display.println(String(hour(dt)) + ":" + ((minute(dt) > 9) ? "" : "0")+ minute(dt));
  switch(id){
    case 800         : display.drawExampleBitmap(sunny_b    , 125, crnty-38, 80, 80, GxEPD_BLACK); break;
    case 701 ... 781 : display.drawExampleBitmap(fog_b      , 125, crnty-38, 80, 80, GxEPD_BLACK); break;
    case 801 ... 802 : display.drawExampleBitmap(suncloud_b , 125, crnty-38, 80, 80, GxEPD_BLACK); break;
    case 803         : display.drawExampleBitmap(cloud2_b   , 125, crnty-38, 80, 80, GxEPD_BLACK); break;
    case 804         : display.drawExampleBitmap(cloud2_b   , 125, crnty-38, 80, 80, GxEPD_BLACK); break;
    case 500 ... 501 : display.drawExampleBitmap(rain0_b    , 125, crnty-38, 80, 80, GxEPD_BLACK); break;
    case 502 ... 531 : display.drawExampleBitmap(rain2_b    , 125, crnty-38, 80, 80, GxEPD_BLACK); break;
    case 300 ... 301 : display.drawExampleBitmap(rain0_b    , 125, crnty-38, 80, 80, GxEPD_BLACK); break;
    case 302 ... 321 : display.drawExampleBitmap(rain2_b    , 125, crnty-38, 80, 80, GxEPD_BLACK); break;
    case 600 ... 622 : display.drawExampleBitmap(snow_b     , 125, crnty-38, 80, 80, GxEPD_BLACK); break;
    case 200 ... 232 : display.drawExampleBitmap(thunder_b  , 125, crnty-38, 80, 80, GxEPD_BLACK); break;       
  }
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(118, crnty+56);
  display.print(round(temp*10)/10,1);
  display.drawCircle(204, crnty+32, 3, GxEPD_BLACK);
}

void fcsthour(int x, int y, long dt, double temp, int id){
 //temp, thunder0, sunny, suncloud, snow, schirm, rain2, rain1, rain0, mooncloud, moon, fog, cloud2 
  switch(id){
    case 800         : display.drawExampleBitmap(    sunny_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;
    case 701 ... 781 : display.drawExampleBitmap(      fog_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;
    case 801 ... 802 : display.drawExampleBitmap( suncloud_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;
    case 803         : display.drawExampleBitmap(   cloud2_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;
    case 804         : display.drawExampleBitmap(   cloud2_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;
    case 500 ... 501 : display.drawExampleBitmap(    rain0_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;
    case 502 ... 531 : display.drawExampleBitmap(    rain2_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;
    case 300 ... 301 : display.drawExampleBitmap(    rain0_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;
    case 302 ... 321 : display.drawExampleBitmap(    rain2_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;
    case 600 ... 622 : display.drawExampleBitmap(     snow_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;
    case 200 ... 232 : display.drawExampleBitmap(  thunder_sml, x+20, y+4, 30, 30, GxEPD_BLACK); break;    
  }
  display.setFont(&FreeMonoBold9pt7b);
  align(x+4, y+6, String(hour(dt)));
  display.print(hour(dt));
  display.print("uhr");
  align(x+22, y+44,String(round(temp),0));
  display.print(round(temp),0); // max temp
  display.drawCircle(x+45, y+33, 2, GxEPD_BLACK);
}

/////////////////////////////////////////////

void fcstday(int x, int y, long dt, double temp_min, double temp_max, int id){
 //temp, thunder0, sunny, suncloud, snow, schirm, rain2, rain1, rain0, mooncloud, moon, fog, cloud2 
  switch(id){
    case 800         : display.drawExampleBitmap(    sunny, x+5, y-2, 60, 60, GxEPD_BLACK); break;
    case 701 ... 781 : display.drawExampleBitmap(      fog, x+5, y-2, 60, 60, GxEPD_BLACK); break;
    case 801 ... 802 : display.drawExampleBitmap( suncloud, x+5, y-2, 60, 60, GxEPD_BLACK); break;
    case 803         : display.drawExampleBitmap(   cloud2, x+5, y-2, 60, 60, GxEPD_BLACK); break;
    case 804         : display.drawExampleBitmap(   cloud2, x+5, y-2, 60, 60, GxEPD_BLACK); break;
    case 500 ... 501 : display.drawExampleBitmap(    rain0, x+5, y-2, 60, 60, GxEPD_BLACK); break;
    case 502 ... 531 : display.drawExampleBitmap(    rain2, x+5, y-2, 60, 60, GxEPD_BLACK); break;
    case 300 ... 301 : display.drawExampleBitmap(    rain0, x+5, y-2, 60, 60, GxEPD_BLACK); break;
    case 302 ... 321 : display.drawExampleBitmap(    rain2, x+5, y-2, 60, 60, GxEPD_BLACK); break;
    case 600 ... 622 : display.drawExampleBitmap(     snow, x+5, y-2, 60, 60, GxEPD_BLACK); break;
    case 200 ... 232 : display.drawExampleBitmap(  thunder0, x+5, y-2, 60, 60, GxEPD_BLACK); break;      
  }
  display.setFont(&FreeMonoBold12pt7b);
  align(x+13, y, dd[weekday(dt)]);
  display.print(dd[weekday(dt)]);
  align(x, y+67, String(round(temp_min),0));
  display.print(round(temp_min),0); //min temp
  display.drawCircle(x+26, y+55, 2, GxEPD_BLACK);
  align(x+40, y+67, String(round(temp_max),0));
  display.print(round(temp_max),0); // max temp
  display.drawCircle(x+66, y+55, 2, GxEPD_BLACK);
}

void getinfo(){
    // wait for WiFi connection
  while(WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
     delay(500);
    }
    Serial.println(WiFi.localIP());
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;
	// get time clock
	    if (http.begin(client, timeurl)) {  // HTTP
      int httpCode = http.GET();
      Serial.print("httpCode: "); Serial.println(httpCode);
      if (httpCode > 0) {
          epochTime = http.getString();
         Serial.println(payload.length());
      }
      http.end();
    }
	//get weather updated
    if (http.begin(client, apiurl)) {  // HTTP
      int httpCode = http.GET();
      Serial.print("httpCode: "); Serial.println(httpCode);
      if (httpCode > 0) {
          payload = http.getString();
         Serial.println(payload.length());
      }
      http.end();
    }
  }
}
void weather() {
DynamicJsonDocument doc(5120);
deserializeJson(doc, epochTime);
crntTime = doc["dt"]+3600;

deserializeJson(doc, payload);
// CORONA
///////////////////////////////////////////////////
   long dt        = doc["covid"]["dt"];
  long  confirmed = doc["covid"]["confirmed"];
 String country   = doc["covid"]["country"];

corona(42, dt, confirmed, country); // send corona cases to epaper
String str  = String(day(dt)) + "." + month(dt) + " " + hour(dt) + ":" + minute(dt) + "  confirmed: " + confirmed + " country: " + country;
Serial.print("covid: ");
Serial.println(str);
// CURRENT
///////////////////////////////////////////////////
           dt = doc["current"]["dt"];
  double temp = doc["current"]["temp"];
     int id  
     = doc["current"]["id"];

dttemp = hour(dt);
current(42, crntTime, temp, id); // send current weather to epaper
  str  = String(day(dt)) + "." + month(dt) + " " + hour(dt) + ":" + minute(dt) + "  " + temp + "°C " + "id: " + id;
  Serial.print("current: ");
  Serial.println(str);
  
// HOURLY
///////////////////////////////////////////////////
for (int i = 0; i <5; i++){
        dt   = doc["hourly"][i]["dt"];
        temp = doc["hourly"][i]["temp"];
          id = doc["hourly"][i]["id"];
        str  = String(day(dt)) + "." + month(dt) + " " + hour(dt) + "h " + temp + "°C " + "id: " + id;
     
fcsthour((i)*80+5, 145, dt, temp, id); // send 5 hous fcst to epaper
  Serial.print("hourly: ");
  Serial.println(str);
}

// DAILY
///////////////////////////////////////////////////
for (int i = 0; i <5; i++){
       dt       = doc["daily"][i]["dt"];
   int temp_min = doc["daily"][i]["min"];
   int temp_max = doc["daily"][i]["max"];
       id       = doc["daily"][i]["id"];

fcstday((i)*80+5, 225, dt, temp_min, temp_max, id); // send 5 days fcst to epaper
  str          = String(day(dt)) + "." + month(dt) + " " + temp_min + "°C-" + temp_max + "°C " + "id: " + id;
  Serial.print("daily: ");
  Serial.println(str);
}
}
