 /*
   Данный код плавно двигает туда-сюда одной сервой (на пине 5)
   Документация: https://alexgyver.ru/servosmooth/
*/

#include <ServoSmooth.h>
ServoSmooth servo;
  #define PIN_PIR A0
uint32_t tmr;
boolean flag;

void setup() {
  Serial.begin(9600);
  servo.attach(5);        // подключить
  servo.setSpeed(530);    // ограничить скорость
  servo.setAccel(0.2);   	  // установить ускорение (разгон и торможение)

   pinMode(PIN_PIR, INPUT);

}

void loop() {
  int pirVal = digitalRead(PIN_PIR);
  servo.tick();

  if (pirVal)
  {
    servo.setTargetDeg( 15);  // двигаем на углы 50 и 120
  }
else
{
   servo.setTargetDeg(100);
}
}
