

#include <Arduino.h>
#include <IRremote.hpp>

unsigned long code,timing_welcome;
unsigned long newCode;
uint32_t timer = 0;

bool steptic;

#include <GyverStepper.h>
GStepper<STEPPER4WIRE> stepper(2048,8, 9, 10, 11);
   int dir;
void setup() {
  // наша задача - при запуске крутить мотор в сторону до нажатия на кнопку
  pinMode(A0, INPUT_PULLUP);  // кнопка на D12 и GND
  IrReceiver.begin(2);
  stepper.setMaxSpeed(500);
  stepper.setAcceleration(500);
 
  stepper.setRunMode(KEEP_SPEED);
  stepper.setSpeedDeg(-20);   // медленно крутимся НАЗАД
  Serial.begin(115200);
  // пока кнопка не нажата
  while(digitalRead(A0)) {    
    stepper.tick();
  }
   stepper.setSpeedDeg(+30);
  stepper.reset();

  // дальше например врубаем FOLLOW_POS
  stepper.setRunMode(FOLLOW_POS);
  delay(5000);
}

void loop() {
steptic= stepper.tick();



if (millis() - timing_welcome > 1000)
{
     if ((stepper.getTargetDeg()==stepper.getCurrentDeg())&&stepper.getTargetDeg()!=0)
{
  delay(1500);
        stepper.setTargetDeg(0);
}
  timer = millis();
  
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

if (stepper.getCurrentDeg()!=0) newCode=0;

 if ((newCode== 1111000001)||(newCode == 16724175)) // атака проведена верно  - сворачиваемся.
   {
   stepper.setTargetDeg(36);
   newCode=0;
   }
  else  if ((newCode == 1111000002)||(newCode == 16718055)) // атака проведена верно  - сворачиваемся.
  {
  stepper.setTargetDeg(36*2);
  newCode=0;
  }
  else  if ((newCode == 1111000003)||(newCode == 16743045)) // атака проведена верно  - сворачиваемся.
  {
  stepper.setTargetDeg(36*3);
  newCode=0;
  }
else  if ((newCode == 1111000004)||(newCode == 16716015)) // атака проведена верно  - сворачиваемся.
  {
  stepper.setTargetDeg(36*4);
   newCode=0;
  }
else  if ((newCode == 1111000005)||(newCode == 16726215)) // атака проведена верно  - сворачиваемся.
  {
  stepper.setTargetDeg(36*5);
  newCode=0;
  }
  else  if ((newCode == 1111000006)||(newCode == 16734885)) // атака проведена верно  - сворачиваемся.
  {
  stepper.setTargetDeg(36*6);
  newCode=0;Ь
  }
  else  if ((newCode == 1111000007)||(newCode == 16728765)) // атака проведена верно  - сворачиваемся.
  {
  stepper.setTargetDeg(36*7);
   newCode=0;
  }
  else  if ((newCode == 1111000008)||(newCode == 16730805)) // атака проведена верно  - сворачиваемся.
  {
  stepper.setTargetDeg(36*8);
   newCode=0;
  }
  else  if ((newCode == 1111000009)||(newCode == 16732845)) // атака проведена верно  - сворачиваемся.
  {
  stepper.setTargetDeg(36*9);
   newCode=0;
  }


}
