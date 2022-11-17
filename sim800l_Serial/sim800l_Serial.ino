#include <SoftwareSerial.h>
#define rxPin D5
#define txPin D6

SoftwareSerial mySerial(rxPin,txPin);

void setup() {
  Serial.begin(9600);
  Serial.println("Software Serial Sketch");
  mySerial.begin(9600);
}
void loop() { // run over and over
  while(mySerial.available()) {
    Serial.write(mySerial.read());
  }
  while(Serial.available()) {
    mySerial.write(Serial.read());
  }
}
