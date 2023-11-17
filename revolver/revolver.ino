// пример с концевиком

#include <Gyverstepper2.h>

Gstepper2<stepper24WIRE> stepper2(2048, 10, 8, 9, 7);
int step_rev;
void setup() {
  // наша задача - при запуске крутить мотор в сторону до нажатия на кнопку
  pinMode(11, INPUT_PULLUP);  // кнопка на D12 и GND
  
  stepper2.setRunMode(KEEP_SPEED);
  stepper2.setSpeedDeg(-70);   // медленно крутимся НАЗАД

  // пока кнопка не нажата
  while(digitalRead(11)) {    
    stepper2.tick();
	// yield();	// для esp8266
  }
 
    stepper2.reset();
  stepper2.setSpeedDeg(60);
  while (stepper2.getCurrent() != 1550) {
   stepper2.tick();
  }

  stepper2.reset();
  stepper2.setSpeedDeg(-30);   // медленно крутимся НАЗАД

  // пока кнопка не нажата
  while(digitalRead(11)) {    
    stepper2.tick();
  }


  // дальше например врубаем FOLLOW_POS
 

    stepper2.reset();
  stepper2.setSpeedDeg(33);
  while (stepper2.getCurrent() != 1380) {
   stepper2.tick();
  }
  stepper2.reset();
   delay(3000);
   stepper2.setSpeedDeg(60);
 stepper2.setRunMode(FOLLOW_POS);
}

void loop() {
  step_rev=700;

  // просто крутим туды-сюды
  if (!stepper2.tick()) {
 
    stepper2.setTarget(step_rev);
  }
  if (stepper2.getCurrent() == step_rev)
  {
    stepper2.reset();
    delay(3000);
  }
}
