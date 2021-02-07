/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservohead; 
Servo myservoeyes; 
Servo myservomouth; // create servo object to control a servo

int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

void setup() {
  myservohead.attach(9); 
  myservoeyes.attach(10); 
  myservomouth.attach(11); // attaches the servo on pin 9 to the servo object
  Serial.begin(9600); 
}

void loop() {
  
               // sets the servo position according to the scaled value
  delay(1500);                           // waits for the servo to get there
             

 myservohead.write(random(10, 170));   
 myservoeyes.write(random(10, 50));   
 myservomouth.write(random(65, 180));   

  // head (up)  10-170
  //eyes 10-50
  //mouth 180 - 65
}
