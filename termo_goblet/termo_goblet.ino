
#include <Tiny4kOLED.h>
#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/power.h>

#include <GyverDS18.h>
GyverDS18Single ds(1);  // пин

uint8_t address[] = { 40, 250, 31, 218, 4, 0, 0, 52 };
uint8_t selected;



// Переменные для шагов и таймеров
bool isStep1 = true;
bool isStep2 = false;
unsigned long step1StartTime = 0;
unsigned long step2StartTime = 0;
const unsigned long intervalStep1 = 20000;  // 30 секунд в миллисекундах для шага 1
const unsigned long intervalStep2 = 20000;  // 30 секунд в миллисекундах для шага 2

// Переменные для мигания светодиода с использованием millis()
unsigned long previousBlinkTime = 0;
const unsigned long blinkInterval = 100;  // Интервал мигания 5 Гц (100 мс)
bool ledState = LOW;

void setup() {
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  ds.requestTemp();  // первый запрос на измерение


  oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);

  //compass.setCalibrationOffsets(-60.00, 1672.00, 138.00);
  //compass.setCalibrationScales(1.02, 0.93, 1.06);
  oled.clear();
  oled.on();
}

void loop() {
  // Получаем значения азимута с компаса

  if (ds.ready()) {       // измерения готовы по таймеру
    if (ds.readTemp()) {  // если чтение успешно

      oled.setFont(FONT8X16P);
      oled.setCursor(0, 1);
      oled.print("Temp: :");
      oled.setFont(FONT8X16P);
      oled.setCursor(25, 1);
      oled.print("         ");

      oled.setFont(FONT8X16P);
      oled.setCursor(34, 1);
      oled.print(ds.getTemp());

    } else {
      oled.setFont(FONT8X16P);
      oled.setCursor(0, 1);
      oled.print("ERROR:");
    }

    ds.requestTemp();  // запрос следующего измерения



  }
}
/*
// Функция мигания светодиода с использованием millis()
void blinkLED() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkTime >= blinkInterval) {
    previousBlinkTime = currentMillis;
    ledState = !ledState;  // Переключаем состояние светодиода
    digitalWrite(ledPin, ledState);
  }
}
*/

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