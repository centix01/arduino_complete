int latchPin = D1; // pin D8 on NodeMCU boards
int clockPin = D2; // pin D5 on NodeMCU boards
int dataPin  = D3;  // pin D7 on NodeMCU

byte leds = 0;    // Variable to hold the pattern of which LEDs are currently turned on or off

void setup() 
{
  // Set all the pins of 74HC595 as OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() 
{
  leds = 0; // Initially turns all the LEDs off, by giving the variable 'leds' the value 0
  updateShiftRegister();
  delay(500);
  for (int i = 0; i < 8; i++) // Turn all the LEDs ON one by one.
  {
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(leds);
    
    bitSet(leds, i);    // Set the bit that controls that LED in the variable 'leds'
    updateShiftRegister();
    delay(1000);
  }
}

/*
 * updateShiftRegister() - This function sets the latchPin to low, then calls the Arduino function 
 * 'shiftOut' to shift out contents of variable 'leds' in the shift register before putting the 'latchPin' high again.
 */
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}
