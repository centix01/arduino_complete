#include <SoftwareSerial.h>
#define rxPin D5
#define txPin D6

const int wt_ms = 100; // wait ms
char Buffer[1000]; 

SoftwareSerial mySerial(rxPin,txPin);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

delay(500);

// mySerial.println("AT+CNETSCAN=1");
// delay(200);
 mySerial.println("AT+COPS?");

 updateSerial(wt_ms);

//  moduleAnswer = moduleAnswer.substring(1);   // does not work without, don't know why!
//  int pos1 = moduleAnswer.indexOf("\"");      // first position of " 
//  int pos2 = moduleAnswer.lastIndexOf("\"");  // last position of "
//  moduleAnswer = moduleAnswer.substring(pos1+1,pos2);
//  Serial.print("The module is connected to ");
  Serial.println(Buffer);
}

void loop() 
{

}

void updateSerial(unsigned int wait_ms){
  String sBuffer = "leer";
  delay(wait_ms);

   while  (!mySerial.available() )  // Schnittstelle noch nichts zu holen
  {
    Serial.println("wait");
    delay(10);
  }
    
  if(mySerial.available()) {
    sBuffer = mySerial.readString();
    sBuffer.toCharArray(Buffer, sBuffer.length());
    Serial.println("got it");
    Serial.println(String(Buffer));
  }
  while(Serial.available()) {
    mySerial.write(Serial.read());
  }
}
