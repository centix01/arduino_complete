#include <SoftwareSerial.h>
#define rxPin D5
#define txPin D6
long myTimer = 0;
String appendString = "";
boolean nextnetscsan = true;
int i = 1;
SoftwareSerial mySerial(rxPin,txPin);

void setup() {
  Serial.begin(9600);
  Serial.println("Software Serial Sketch");
  mySerial.begin(9600);

//  Serial.setTimeout(1000);
}
void loop() { // run over and over
 if(nextnetscsan) {

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
      Serial.println(i);
      i++;
      Serial.println(appendString);
      appendString = "";
      nextnetscsan = true;
      delay(5000);
    }
  }


}
