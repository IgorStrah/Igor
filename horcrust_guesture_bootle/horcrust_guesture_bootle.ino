
#include <TinyWireM.h>
#include <Tiny4kOLED.h>
#include "RevEng_PAJ7620.h"
#define Wire TinyWireM
// Create gesture sensor driver object
RevEng_PAJ7620 sensor = RevEng_PAJ7620();

#include <tinyNeoPixel.h>
#define NUM_LEDS 4
#define NEOPIXEL_PIN 1  // PB1 (physical pin 6)
tinyNeoPixel strip = tinyNeoPixel(NUM_LEDS, 1, NEO_GRB + NEO_KHZ800);

// Кол-во пикселей и пин

// Цвета
uint32_t red = strip.Color(255, 0, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t black = strip.Color(0, 0, 0);
// Состояние
unsigned long lastInputTime = 0;
int currentLed = 0;
bool processActive = false;

void setup() {
    noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();

    strip.begin();
 strip.setBrightness(150);
 for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, red);
       strip.show();\
           delay(5);
  }
  strip.show();
  delay(1000);

    TinyWireM.begin();
sensor.begin();         // return value of 0 == success
  delay(100);
sensor.begin();  
  delay(100);
}


void loop() {
  Gesture gesture;                  // Gesture is an enum type from RevEng_PAJ7620.h
  gesture = sensor.readGesture();   // Read back current gesture (if any) of type Gesture


  if (gesture) {
        handleInput(gesture);
    delay(100); // пауза для читаемости
  }

   // Если время вышло и процесс активен — откатываемся на 1 этап
  if (processActive && (millis() - lastInputTime >= 5000)) {
    revertStep();
  }
}

// --- Обработка ввода ---
void handleInput(int input) {
    if (input == 6)
  {  (input =5); } 
  if (input == 5&&currentLed==4) { // Полное выключение
    fadeToBlack();
    processActive = false;
    currentLed = 0;
    return;
  }

  if (!processActive && input == 1) { // Начало последовательности
    fadeToBlue(0);
    currentLed = 1;
    processActive = true;
    lastInputTime = millis();
  } 
  else if (processActive) {
    if (input == currentLed + 1) { // Правильный следующий этап
      fadeToBlue(input - 1);
      currentLed = input;
      lastInputTime = millis();
    } 
    else { // Ошибка! Откат на 1 этап
      revertStep();
    }
  }
}

// --- Откат на предыдущий этап ---
void revertStep() {
  if (currentLed > 0) {
    strip.setPixelColor(currentLed - 1, red); // Возвращаем предыдущий светодиод к красному
    strip.show();
    currentLed--;
    lastInputTime = millis(); // Перезапускаем таймер
  } 
  else { // Если этапов не осталось — сброс
    processActive = false;
  }
}

// --- Вспомогательные функции ---
void allRed() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, red);
        strip.show();
    delay(5);
  }
}

void fadeToBlue(int led) {
  for (int i = 0; i <= 255; i++) {
    strip.setPixelColor(led, strip.Color(255 - i, 0, i));
    strip.show();
    delay(5);
    
  }
}

void fadeToBlack() {
  for (int j = 50; j >= 0; j--) {
    strip.setBrightness(j);
    strip.show();
    delay(30);
  }
  strip.setBrightness(0);
//  allRed();
  strip.show();
}

