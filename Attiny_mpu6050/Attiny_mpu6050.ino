#include <Wire.h>
#include <Tiny4kOLED.h>

#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/power.h>

#define MPU9250_ADDR 0x68 // Адрес MPU9250
#define OUTPUT_PIN PB1    // Пин для активации (PB0)

const float threshold = 1.3;   // Порог ускорения для резкого движения (g)
const int debounceTime = 500;  // Минимальное время между движениями (мс)
const float orientationThreshold = 0.7; // Ориентация: ось X ~ 1 g

unsigned long lastMoveTime = 0;
int moveCount = 0, first;


// Функция записи в регистр MPU9250
void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU9250_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// Функция чтения 16-битного значения из MPU9250
int16_t read16bit(uint8_t reg) {
  Wire.beginTransmission(MPU9250_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU9250_ADDR, 2);

  int16_t value = Wire.read() << 8 | Wire.read();
  return value;
}

void setup() {
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  pinMode(OUTPUT_PIN, OUTPUT);
 digitalWrite(OUTPUT_PIN, HIGH);
          delay(1100); // Задержка для проверки (5 секунд)
 Wire.begin();
          digitalWrite(OUTPUT_PIN, LOW);
          delay(1100);

  // Инициализация MPU9250
  writeRegister(0x6B, 0x80);
  writeRegister(0x6B, 0x00); // Выход из спящего режима
  writeRegister(0x1C, 0x00); // Устанавливаем диапазон акселерометра ±2g

       digitalWrite(OUTPUT_PIN, HIGH);
      delay(1100); // Задержка для проверки (5 секунд)

/*
  oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
  oled.clear();
  oled.on();
  */
  first=0;
}




void loop() {
 
  // Чтение ускорений по осям
  int16_t ax_raw = read16bit(0x3B);
  int16_t ay_raw = read16bit(0x3D);
  int16_t az_raw = read16bit(0x3F);

  // Конвертация значений в g (1 g ≈ 16384)
  float ax = ax_raw / 16384.0;
  float ay = ay_raw / 16384.0;
  float az = az_raw / 16384.0;
/*
  oled.clear();
  oled.setFont(FONT8X16P);
  oled.setCursor(0, 1);
  oled.print("ax: ");
  oled.setFont(FONT8X16P);
  oled.setCursor(28, 1);
  oled.print(ax);
*/
  // Проверка ориентации: ось X должна быть направлена вниз или вверх
  if (abs(ax) > orientationThreshold && abs(ay) < 0.4 && abs(az) < 0.4) {
    if (first==0)
{
   digitalWrite(OUTPUT_PIN, LOW);
   delay(100); // Задержка для проверки (5 секунд)
   digitalWrite(OUTPUT_PIN, HIGH);
   first=1;
}
    // Если ось X смотрит вниз или вверх, проверяем резкие движения

    if (abs(ax) > threshold) {
      unsigned long currentTime = millis();

      // Проверяем, прошло ли достаточно времени с последнего движения
      if (currentTime - lastMoveTime > debounceTime) {
        lastMoveTime = currentTime;
        moveCount++;
         digitalWrite(OUTPUT_PIN, LOW);
          delay(100); // Задержка для проверки (5 секунд)
        digitalWrite(OUTPUT_PIN, HIGH);


        // Если зафиксировано 4 движения, включаем пин 4
        if (moveCount >= 3) {
          //digitalWrite(OUTPUT_PIN, HIGH);
          // delay(5000); // Задержка для проверки (5 секунд)
          digitalWrite(OUTPUT_PIN, LOW);
           delay(5000); // Задержка для проверки (5 секунд)
          moveCount = 0; // Сброс счетчика
          writeRegister(0x6B, 0x40);
          enterDeepSleep();
        }
      }
    }else {
    // Если ориентация нарушена, сбрасываем счетчик движений
   moveCount=0;
  }
  } 

 
}

void enterDeepSleep() {
  // Отключаем ненужные модули для экономии энергии
  power_all_disable();

  // Устанавливаем режим сна "Power-down"
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Включаем режим сна
  sleep_enable();

  // Переходим в сон
  sleep_cpu();

  // После пробуждения (если оно произойдёт) выполнение начнётся отсюда, но для ATtiny85 это не нужно
  sleep_disable();
}
