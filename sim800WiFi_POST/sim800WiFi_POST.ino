/*  More information at: https://www.aeq-web.com/
 *  ESP32 SIM800 HTTP POST TEST | V 1.0_2020-AUG-18
 */
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
 
#define SIM800L_RX     D5
#define SIM800L_TX     D6
#define mofset         D1
#define led            D4

String apn = "pinternet.interkom.de";                    //APN
String apn_u = "";                     //APN-Username
String apn_p = "";                     //APN-Password
String url = "http://www.centemero.com/sim800.php";  //URL of Server

String appendString = "";
String timeString ="";

SoftwareSerial Serial2(SIM800L_RX,SIM800L_TX);

void setup()
{
//  pinMode(led, OUTPUT);
//  digitalWrite(led,HIGH);
  pinMode(mofset, OUTPUT);
  digitalWrite(mofset,HIGH);
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  delay(100);
 
  Serial.println("ESP32+SIM800L AT CMD Test");
  Serial2.begin(9600);
  delay(15000);
  while (Serial2.available()) {
    Serial.write(Serial2.read());
  }
  delay(2000);
  
}


void loop() {
  gsm_config_gprs();
  
  String wifname;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t* bssid;
  int32_t channel;
  bool hidden;
  int scanResult; 
  char wifis[255];
  String wifiappend = "";

  scanResult = WiFi.scanNetworks(false,true);
  if (scanResult > 0) {
    for (int8_t i = 0; i < scanResult; i++) {
      WiFi.getNetworkInfo(i, wifname, encryptionType, rssi, bssid, channel, hidden);
      sprintf(wifis, "{%02X:%02X:%02X:%02X:%02X:%02X,%d,0}\n",bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], rssi);  
      wifiappend += wifis;
      yield();
    }
  }

  gsm_send_serial("AT+CCLK?", 3000);
  gsm_http_post(timeString + "\n");
  timeString = "";

  netscan();
  if(appendString.indexOf("OK") > 0 & appendString.length() > 0) {
  gsm_http_post(appendString + "\n");
  appendString = "";
}

  gsm_http_post(wifiappend + "\n");

Serial.println(" --- SIM800 Power OFF--- ");
  digitalWrite(mofset,LOW);
  delay(90000);
  Serial.println(" --- SIM800 Power ON--- ");
  digitalWrite(mofset,HIGH);
  delay(10000);
}

void gsm_http_post( String postdata) {
  postdata.replace("\n\n\n\n","\n");
  postdata.replace("\n\n\n"  ,"\n");
  postdata.replace("\n\n"    ,"\n");
  postdata.replace("OK\n"    ,"\n");
  Serial.println(" --- Start GPRS & HTTP --- ");
  gsm_send_serial("AT+SAPBR=1,1"          , 2000);
  gsm_send_serial("AT+SAPBR=2,1"          , 2000);
  gsm_send_serial("AT+HTTPINIT"           , 2000);
  gsm_send_serial("AT+HTTPPARA=CID,1"     , 2000);
  gsm_send_serial("AT+HTTPPARA=URL," + url, 3000);
  gsm_send_serial("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded", 3000);
  gsm_send_serial("AT+HTTPDATA=1800,6000", 3000);
//digitalWrite(led,LOW);
  gsm_send_serial(postdata         , 3000);
  gsm_send_serial("AT+HTTPACTION=1", 3000);
  gsm_send_serial("AT+HTTPREAD"    , 3000);
//digitalWrite(led,LOW);
  gsm_send_serial("AT+HTTPTERM"    , 3000);
  gsm_send_serial("AT+SAPBR=0,1"   , 3000);
//  digitalWrite(led,HIGH);
}

void gsm_config_gprs() {
  Serial.println(" --- CONFIG GPRS --- ");
  gsm_send_serial("AT+SAPBR=3,1,Contype,GPRS", 3000);
  gsm_send_serial("AT+SAPBR=3,1,APN," + apn  , 3000);
  if (apn_u != "") {
    gsm_send_serial("AT+SAPBR=3,1,USER," + apn_u, 3000);
  }
  if (apn_p != "") {
    gsm_send_serial("AT+SAPBR=3,1,PWD," + apn_p, 3000);
  }
}

void netscan() {
  gsm_send_serial("AT+CNETSCAN=1", 2000);
  gsm_send_serial("AT+CNETSCAN", 15000);
}

void gsm_send_serial(String command, int waitms) {
  Serial.println("Send ->: " + command);
  Serial2.println(command);
  long wtimer = millis();
  while (wtimer + waitms > millis()) {
    while (Serial2.available()) {
//      Serial.write(Serial2.read());
String readstring = Serial2.readString();
Serial.println(readstring);
if(readstring.indexOf("Operator") > 0) {
  appendString +=  '\n' + readstring;
}
if(readstring.indexOf("+CCLK: ")) {
  timeString +=  '\n' + readstring;
}
  }
  }
  Serial.println();
}
