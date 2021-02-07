/*
  Controlling a servo position using a potentiometer (variable resistor)
  by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

  modified on 8 Nov 2013
  by Scott Fitzgerald
  http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo HEAD,LEFT_HAND,RIGHT_HAND;  // create servo object to control a servo
int HEAD_val = 150;
int LEFT_HAND_val =0;
int RIGHT_HAND_val =150;

int potpin = 0;  // analog pin used to connect the potentiometer
#define RIGHT_HAND_PIN    5 // 110 раскрыто 0 закрыто
#define LEFT_HAND_PIN    6 // 170 раскрыто 60 закрыто
#define HEAD_PIN       9 // 0 - вверх 90 в низ


void setup() {
 // HEAD.attach(HEAD_PIN);  // attaches the servo on pin 9 to the servo object
 // LEFT_HAND.attach(LEFT_HAND_PIN);  // attaches the servo on pin 9 to the servo object
  RIGHT_HAND.attach(RIGHT_HAND_PIN);  // attaches the servo on pin 9 to the servo object
 // HEAD.write(150);
 // LEFT_HAND.write(0);
  RIGHT_HAND.write(100);
  delay(1000);
}

void loop() {

// open

/*
  if (40<=HEAD_val)
  {
    HEAD_val--;                             // scale it to use it with the servo (value between 0 and 180)
    HEAD.write(HEAD_val);                  // sets the servo position according to the scaled value
    delay(20);                            // waits for the servo to get there
  }
  else
  {HEAD.detach();}
*/

  if (140>=LEFT_HAND_val)
  {
    LEFT_HAND_val++;                                  // scale it to use it with the servo (value between 0 and 180)
    LEFT_HAND.write(LEFT_HAND_val);                  // sets the servo position according to the scaled value
                                                    // waits for the servo to get there
  }
  else
  {  LEFT_HAND.detach(); }

  if (10<=RIGHT_HAND_val)
  {
    RIGHT_HAND_val--;// scale it to use it with the servo (value between 0 and 180)
    RIGHT_HAND.write(RIGHT_HAND_val);                  // sets the servo position according to the scaled value
                             // waits for the servo to get there
  }
  else
  {  RIGHT_HAND.detach(); }

  
  if (1<=RIGHT_HAND_val)
  {
    RIGHT_HAND_val--;
     RIGHT_HAND.write(RIGHT_HAND_val);
  }
  else
  {  RIGHT_HAND.detach(); }

    
  delay(50); 
}
