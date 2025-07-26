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
unsigned long newCode;

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
    if (IrReceiver.decode()) {
        newCode=0;
      code=0;
      code=IrReceiver.decodedIRData.decodedRawData;

      for (int i = 0; i < 32; i++) {
          // Extract the ith bit from the old code
          unsigned long bit = (code >> (31 - i)) & 1;

          // Set the ith bit in the new code
          newCode |= (bit << i);
        }
        Serial.println(newCode);
       
        delay(100);
         //IrReceiver.decodedIRData.decodedRawData();
        IrReceiver.resume();
    }
  
 

                
   if ((newCode== 1111000001)||(newCode == 16724175)) // атака проведена верно  - сворачиваемся.
   {
     myservo.attach(2);  // attaches the servo on pin 9 to the servo object
  potpin=135;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
   }
 
  else  if ((newCode == 1111000005)||(newCode == 16726215)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object  
  potpin=90;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }
  else  if ((newCode == 1111000006)||(newCode == 16734885)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object  
  potpin=45;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }
 
  else  if ((newCode == 1111000008)||(newCode == 16730805)) // атака проведена верно  - сворачиваемся.
  {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object  
  potpin=2;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(800);                           // waits for the servo to get there
  potpin=180;
  myservo.write(potpin);                  // sets the servo position according to the scaled value
  delay(200);
  myservo.detach();  // attaches the servo on pin 9 to the servo object
  }
    if (IrReceiver.decode()) {
        newCode=0;
      code=0;
      code=IrReceiver.decodedIRData.decodedRawData;
 
        IrReceiver.resume();
    }

  
newCode=0;
}
