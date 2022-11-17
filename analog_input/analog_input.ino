

int sensorPin = A0; 
int sensorValue = 0; 
double val = 0; 


void setup() {
  // put your setup code here, to run once:
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // prints title with ending line break
  Serial.println("ASCII Table ~ Character Map");
}

void loop() {
  sensorValue = analogRead(sensorPin);  
Serial.print(sensorValue);
val = map(sensorValue, 0, 1023, 0, 3.3);
Serial.print(" - ");
Serial.println(val);
delay(500);
}
