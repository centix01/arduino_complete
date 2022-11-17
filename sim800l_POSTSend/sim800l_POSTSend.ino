#include <SoftwareSerial.h>
#define rxPin D5
#define txPin D6
long myTimer = 0;
String appendString = "";
boolean nextnetscsan = true;

SoftwareSerial mySerial(rxPin,txPin);

void setup() {
  Serial.begin(9600);
  Serial.println("Software Serial Sketch");
  mySerial.begin(9600);

//  Serial.setTimeout(1000);
}
void loop() { // run over and over
 mySerial.println("AT+CIICR");
 delay(200);
 mySerial.println("AT+CSTT=\"pinternet.interkom.de\",\"\",\"\""); // set APN
 delay(200);
 mySerial.println("AT+CIICR"); // gprse connection
 delay(200);
 mySerial.println("AT+CIFSR"); // get ip

 mySerial.println("AT+CIPSTART=\"TCP\",\"centemero.com\",80"); //call api site
 delay(200);
 mySerial.println("AT+CIPSEND=63);
 mySerial.println("GET centemero.com/gps.php HTTP/1.0");
 mySerial.write(0x0d);
 mySerial.write(0x0a);
 mySerial.write(0x1a);
 delay(5000);
 mySerial.println("AT+CIPCLOSE");

 delay(10000);
}
