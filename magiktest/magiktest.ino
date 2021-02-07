/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>
#include <IRremote.h>
int RECV_PIN = 3;
IRrecv irrecv(RECV_PIN);
decode_results results;


unsigned long code;

Servo myservo;  // create servo object to control a servo

int potpin = 180;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

void setup() {
 // myservo.attach(2);  // attaches the servo on pin 9 to the servo object
    Serial.begin(115200);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");

}

void loop() {
code="";
 delay(100);
 if (irrecv.decode(&results)) {
    Serial.println(results.value);
    irrecv.resume(); // Receive the next value
    code= results.value;
  }
  
 

  
   if ((code== 1111000001)||(code == 16753245)) // атака проведена верно  - сворачиваемся.
   {
     myservo.attach(2);  // attaches the servo on pin 9 to the servo object
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
   }
  else  if ((code == 1111000002)||(code == 16736925)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object
  potpin=160;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }
  else  if ((code == 1111000003)||(code == 16769565)) // атака проведена верно  - сворачиваемся.
  {  
  myservo.attach(2);  // attaches the servo on pin 9 to the servo objec
  potpin=140;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }

  else  if ((code == 1111000004)||(code == 16720605)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object
  potpin=120;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }
  else  if ((code == 1111000005)||(code == 16712445)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object  
  potpin=100;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }
  else  if ((code == 1111000006)||(code == 16761405)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object  
  potpin=80;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }
  else  if ((code == 1111000007)||(code == 16769055)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object  
  potpin=60;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }
  else  if ((code == 1111000008)||(code == 16754775)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object  
  potpin=40;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }
  else  if ((code == 1111000009)||(code == 16748655)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object  
  potpin=20;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }
  else  if ((code == 1111000010)||(code == 16750695)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object
  potpin=0;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }

if (irrecv.decode(&results)) {
    Serial.println(results.value);
    irrecv.resume(); // Receive the next value
    code= results.value;
  }

}
