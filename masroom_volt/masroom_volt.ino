#include <Wire.h>
#include <QMC5883LCompass.h>  // Библиотека для компаса QMC5883L
#include <Tiny4kOLED.h>
#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/power.h>


QMC5883LCompass compass;
const int ledPin = PB1; 
const int ledPin2 = PB4;
int azimuth = 0;

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

  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);


 digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW); 
  delay(1000);
  digitalWrite(ledPin, HIGH);

  digitalWrite(ledPin2, HIGH);
  delay(1000);
  digitalWrite(ledPin2, LOW); 
   oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
  // Инициализация компаса
 compass.init();
//compass.setCalibrationOffsets(-60.00, 1672.00, 138.00);
//compass.setCalibrationScales(1.02, 0.93, 1.06);
  oled.clear();
  oled.on();
}

void loop() {
  // Получаем значения азимута с компаса
  compass.read();
  azimuth = compass.getAzimuth();

  if (azimuth==0)
  {digitalWrite(ledPin2, LOW); }
  else
  {
    digitalWrite(ledPin2, HIGH); 
  }
 oled.setFont(FONT8X16P);
  oled.setCursor(0, 1);
  oled.print("A:");
  oled.setFont(FONT8X16P);
  oled.setCursor(25, 1);
  oled.print("         ");

  oled.setFont(FONT8X16P);
  oled.setCursor(24, 1);
  oled.print(azimuth);


 
  // Шаг 1: Проверка азимута от 20 до 40 и ожидание 30 секунд
  if (isStep1) {
    if (azimuth >= 95 && azimuth <= 115) {
        
      if (step1StartTime == 0) {
        step1StartTime = millis();  // Начинаем отсчет времени, если только что вошли в диапазон
      }
     // blinkLED();  // Мигание светодиода с частотой 5 Гц
      // Проверка, удерживается ли азимут в диапазоне 20-40 в течение 30 секунд
      if (millis() - step1StartTime >= intervalStep1) {
        isStep1 = false;
        isStep2 = true;
        step2StartTime = millis();  // Начинаем отсчет времени для шага 2
         digitalWrite(ledPin, LOW);  
         delay(1000);
        digitalWrite(ledPin, HIGH);  
      }
    } else {
      step1StartTime = 0;  // Сброс таймера, если азимут вышел за пределы диапазона
       digitalWrite(ledPin, HIGH);  
    }
  }
  
  // Шаг 2: Проверка азимута от 130 до 160
  if (isStep2) {
    if (azimuth >= 160 && azimuth <= 170) {
      blinkLED(); 
      // Проверка, удерживается ли азимут в диапазоне 30 секунд
      if (millis() - step2StartTime >= intervalStep2) {
        digitalWrite(ledPin, LOW);  // Выключаем светодиод
        isStep2 = false;
          oled.clear();
          oled.off();
           enterDeepSleep();
      }
    } else {
      // Если азимут выходит из диапазона 130-160, сбрасываем таймер
      step2StartTime = millis();
      digitalWrite(ledPin, HIGH); 
      // Если время ожидания истекло
      if (millis() - step2StartTime >= intervalStep2) {
        isStep1 = true;
        isStep2 = false;
        step1StartTime = 0;
        digitalWrite(ledPin, HIGH); 
      }
    }
  }
  
}

// Функция мигания светодиода с использованием millis()
void blinkLED() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousBlinkTime >= blinkInterval) {
    previousBlinkTime = currentMillis;
    ledState = !ledState;  // Переключаем состояние светодиода
    digitalWrite(ledPin, ledState);
 
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