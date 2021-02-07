/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInOutSerial
*/

// These constants won't change. They're used to give names to the pins used:

const int analogOutPin2 = 9; // Analog output pin that the LED is attached to
const int analogOutPin = 10; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
}

void loop() {
  // read the analog in value:
  analogWrite(analogOutPin, 170);
  analogWrite(analogOutPin2, 0);

  delay(5500);
  analogWrite(analogOutPin, 90);
  analogWrite(analogOutPin2, 0);

  delay(4000);
  analogWrite(analogOutPin2, 90);
  analogWrite(analogOutPin, 0);
  delay(7500);


//  stop
  analogWrite(analogOutPin2,0);
  analogWrite(analogOutPin, 0);
  delay(200);

  
  analogWrite(analogOutPin, 190);
  analogWrite(analogOutPin2, 0);
  delay(800);

  analogWrite(analogOutPin, 0);
  analogWrite(analogOutPin2, 120);
  delay(600);

  
  analogWrite(analogOutPin, 190);
  analogWrite(analogOutPin2, 0);
  delay(800);

  analogWrite(analogOutPin, 0);
  analogWrite(analogOutPin2, 90);
  delay(500);
  
  analogWrite(analogOutPin, 0);
  analogWrite(analogOutPin2, 120);
  delay(900);




  
  // print the results to the Serial Monitor:
  Serial.println(outputValue);

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(2);
}
