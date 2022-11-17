#include "Wire.h"
#include "Adafruit_INA219.h"


Adafruit_INA219 ina219;


float capacity_mAh = 0;
String alert = "";

void setup() {
  pinMode(D5,OUTPUT);
  
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  //setCalibration_32V_2A ()
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
//  ina219.setCalibration_16V_400mA();

  Serial.print("Alert"); Serial.print("\t"); 
  Serial.print("BV"); Serial.print("\t"); // Bus Voltage
  Serial.print("SV"); Serial.print("\t"); // Shunt Voltage
  Serial.print("LV"); Serial.print("\t"); // Load Voltage
  Serial.print("mah"); Serial.print("\t"); // Capacity
  Serial.print("C"); Serial.print("\t");  // Current
  Serial.println("P");  // Power

  digitalWrite(D5,HIGH);

}

void loop() {
  
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  current_mA = ina219.getCurrent_mA();
  capacity_mAh += current_mA/3600;
  power_mW = ina219.getPower_mW();
  
//  if (busvoltage <= 3) {
//    digitalWrite(D5,LOW);
//    alert = "LOW VOLTAGE <= 3.2N";
//  } else {
//    digitalWrite(D5,HIGH);
//  }
    
  Serial.print(alert); Serial.print("\t"); 
  Serial.print(busvoltage); Serial.print("\t"); 
  Serial.print(shuntvoltage); Serial.print("\t");
  Serial.print(loadvoltage); Serial.print("\t");
  Serial.print(current_mA); Serial.print("\t");
  Serial.print(capacity_mAh); Serial.print("\t");
  Serial.println(power_mW);

  delay(1000);
}
