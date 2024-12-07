#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>

// Инициализация датчика цвета TCS34725
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

// Инициализация сервопривода
Servo myservo;
int servoPin = 9; // Пин, к которому подключен сервопривод
int pos = 0;
void setup() {
  Serial.begin(115200);
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // остановка программы если датчик не найден
  }

  myservo.attach(servoPin); // Подключаем сервопривод
  myservo.write(110); // Устанавливаем начальную позицию 180 градусов
  delay(1000);
  myservo.detach();
}

void loop() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  // Преобразование значений в освещенность (Lux)
  uint16_t lux = tcs.calculateLux(r, g, b);

  Serial.print("Lux: "); Serial.println(lux);

  // Проверка условия для значения Lux
    if ( lux < 30) {
    delay(500); // Небольшая задержка перед первым контрольным измерением

    // Выполнение 3 контрольных измерений
    bool stableLowLux = true;
    for (int i = 0; i < 3; i++) {
      delay(500); // Задержка между контрольными измерениями
      tcs.getRawData(&r, &g, &b, &c);
      lux = tcs.calculateLux(r, g, b);
      Serial.print("Control Lux: "); Serial.println(lux);
       if (lux >= 120) {
        stableLowLux = false;
        break;
      }
    }

    // Если все 3 контрольных измерения подтвердили низкий уровень Lux
    if (stableLowLux) {
      Serial.println("Detected stable low Lux, moving servo");
      moveServo();
      moveServo();
      while(1);
    }
  }

  delay(1000); // Задержка для снижения частоты опроса датчика
}

void moveServo() {
   myservo.attach(servoPin); // Подключаем сервопривод
for (pos = 110; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
  }
  delay(3000);
  for (pos = 180; pos >= 110; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
  }
  delay(1000);
  myservo.detach();
}
