#include <Arduino.h>
#include <IRremote.hpp>
#include <Servo.h>

Servo myservo0;  // create servo object to control a servo
Servo myservo1;  // create servo object to control a servo
unsigned long code,timing_welcome;
unsigned long newCode;
uint32_t timer = 0;
int step=0, position, positionold;
bool steptic;


void setup() {
  // наша задача - при запуске крутить мотор в сторону до нажатия на кнопку
  pinMode(A0, OUTPUT);  // кнопка на D12 и GND
  pinMode(A1, INPUT_PULLUP);  // кнопка на D12 и GND
   digitalWrite(A0,0);
  IrReceiver.begin(2);
  Serial.begin(115200);
 position=0;

    delay(500);
    myservo0.attach(8);  //Cursor
    myservo1.attach(9);  // Coim
     myservo1.write(180); 
     myservo0.write(position); 
     delay(1000); 
     myservo1.detach();
   //  myservo0.detach();
    myservo1.attach(9);  // Coim                  
    myservo1.write(180);     
    delay(800);  
}
void loop() {

if (millis() - timing_welcome > 1000)
{

  Serial.print("nwe ");
  Serial.print(position);
    Serial.print(" step  ");
  Serial.println(step);

if (step==2&&position<=60)
{step=1;}

if (step==1&&position<=1)
{step=0;}

if (step!=0&&position>=0)
{
position=position-5;
}
if (position !=positionold)
{
//myservo0.attach(8);  //Cursor 
myservo0.write(position); 
//delay(100);
//myservo0.detach();

positionold=position;
}
  timing_welcome = millis();
}


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

if (step==0 && ((newCode == 1111000001)||(newCode == 16724175)))
{
step=1;
position=60;
 Serial.print("old  ");
  Serial.println(positionold);
  Serial.print("nwe ");
  Serial.println(position);
    newCode=0;
}
if (step==1 && ((newCode == 1111000002)||(newCode == 16718055)))
{
step=2;
position=120;
  newCode=0;
}
if (step==2 && ((newCode == 1111000008)||(newCode == 16730805)))
{
  position=175;
       Serial.println("On");
  myservo0.write(position); 
  delay(100);   
    myservo1.attach(9);  // Coim
    delay(100);   
    myservo1.write(0);              
    delay(800);                       
    myservo1.write(180);     
    delay(800);  
        myservo1.write(0);              
    delay(800);                       
    myservo1.write(180);     
    delay(800);  
        myservo1.write(0);              
    delay(800);                       
    myservo1.write(180);     
    delay(800);           
     myservo1.detach();

  digitalWrite(A0,1);
  delay(2000);
  digitalWrite(A0,0);
  newCode=0;


  while(position>=0)
  {
     
      if (digitalRead(A1)==0)
      {
  position--;
  delay(1000);
      }
       myservo0.write(position); 
  }
}


}


