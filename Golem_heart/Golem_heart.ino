#include <Wire.h>
#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/power.h>

#define MPU9250_ADDR 0x68 // Адрес MPU9250

int bright=255;
const float threshold = 1.3;   // Порог ускорения для резкого движения (g)
const int debounceTime = 200;  // Минимальное время между движениями (мс)
const int ledcloc = 30;  // Минимальное время между движениями (мс)
const float orientationThreshold = 0.65; // Ориентация: ось X ~ 1 g

unsigned long lastMoveTime = 0;
unsigned long lastLcdTime = 0;
int moveCount = 0, first;
#define STRIP_PIN PB3     // пин ленты
#define NUMLEDS 3      // кол-во светодиодов

#define COLOR_DEBTH 3
#include <microLED.h>   // подключаем библу
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;

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

 Wire.begin();


  // Инициализация MPU9250
  writeRegister(0x6B, 0x80);
  writeRegister(0x6B, 0x00); // Выход из спящего режима
  writeRegister(0x1C, 0x00); // Устанавливаем диапазон акселерометра ±2g


  first=0;
   strip.setBrightness(250);
    strip.fill(mWheel8(200));
    strip.show(); 
    delay(100);


}




void loop() {
  unsigned long currentTime = millis();
  // Чтение ускорений по осям
  int16_t ax_raw = read16bit(0x3B);
  int16_t ay_raw = read16bit(0x3D);
  int16_t az_raw = read16bit(0x3F);

  // Конвертация значений в g (1 g ≈ 16384)
  float ax = ax_raw / 16384.0;
  float ay = ay_raw / 16384.0;
  float az = az_raw / 16384.0;



if (abs(az) > orientationThreshold&&first==1)
{
     strip.fill(mWheel8(100));
    strip.setBrightness(bright);
      strip.show();   // вывод
   bright=255;
   first=0;
    moveCount=0;
    delay(500);
  
}
  // Проверка ориентации: ось X должна быть направлена вниз или вверх
  if (abs(ax) > orientationThreshold && abs(ay) < 0.4 && abs(az) < 0.4) {
    if (first==0)
{
     strip.fill(mWheel8(200));
   bright=200;
   first=1;
   
}
    // Если ось X смотрит вниз или вверх, проверяем резкие движения

    if (abs(ax) > threshold) {
     

      // Проверяем, прошло ли достаточно времени с последнего движения
      if (currentTime - lastMoveTime > debounceTime) {
        lastMoveTime = currentTime;
        moveCount++;
      bright=bright-10;


        // Если зафиксировано 4 движения, включаем пин 4
        if (moveCount >= 12) {
        bright=0;
        strip.setBrightness(bright);
        strip.show();
        strip.clear();
        delay(15000);
          moveCount = 0; // Сброс счетчика
          writeRegister(0x6B, 0x40);
          enterDeepSleep();
        }
      }
    }
    else {
    // Если ориентация нарушена, сбрасываем счетчик движений
  
  }
  } 

 if (currentTime - lastLcdTime > ledcloc) {
        lastLcdTime = currentTime;  
        strip.setBrightness(bright);
strip.show();   // вывод
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
