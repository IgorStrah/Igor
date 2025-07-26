#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define LED_PIN 4         // PB4
#define MIC_PIN 3        // PB2
#define NUM_LEDS 1

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const unsigned long calibrationTime = 300;     // 3 сек
const unsigned long silenceThresholdTime = 3000; // 30 сек тишины
const unsigned long transitionTime = 15000;     // переход длится 30 сек

float baseNoise = 0;
float silenceThreshold = 0;
unsigned long silenceStartTime = 0;
unsigned long transitionStartTime = 0;
bool inTransition = false;

void setup() {
  noInterrupts();
  CLKPR = 0x80;
  CLKPR = 0x00;  // no prescaler
  interrupts();

  strip.begin();
  strip.show();

  pinMode(MIC_PIN, INPUT);

  // Калибровка
  long sum = 0;
  int count = 0;
  unsigned long startCal = millis();
  while (millis() - startCal < calibrationTime) {
    sum += analogRead(MIC_PIN);
    count++;
    delay(10);
  }
  baseNoise = (float)sum / count;
  silenceThreshold = baseNoise +45;\
  strip.setPixelColor(0, strip.Color(0, 0, 255)); // синий
  strip.show();
}

void loop() {
  int mic = analogRead(MIC_PIN);
  unsigned long now = millis();
  if (!inTransition) {
    if (mic < silenceThreshold) {
      if (silenceStartTime == 0) silenceStartTime = now;
      if (now - silenceStartTime >= silenceThresholdTime) {
        // Начинаем переход
              strip.setPixelColor(0, strip.Color(222, 0, 255)); // синий
      strip.show();
        inTransition = true;
        transitionStartTime = now;
      }
    } else {
      silenceStartTime = 0;
    }
  } else {
    // Переход активен
    if (mic >= silenceThreshold) {
      // Слишком громко — прерываем
      inTransition = false;
      silenceStartTime = 0;
      strip.setPixelColor(0, strip.Color(0, 0, 255)); // синий
      strip.show();
      return;
    }

    // Вычисляем прогресс
    float progress = float(now - transitionStartTime) / transitionTime;
    if (progress >= 1.0) {
      // Анимация завершена
      strip.clear();
      strip.show();
      goToSleep();
      return;
    }

    // Плавное изменение от синего к зелёному с затуханием
    uint8_t green = (uint8_t)(255 * progress);
    uint8_t blue = (uint8_t)(255 * (1.0 - progress));
    uint8_t brightness = (uint8_t)(255 * (1.0 - progress));

    strip.setPixelColor(0, strip.Color(0, green * brightness / 255, blue * brightness / 255));
    strip.show();
  }

  delay(50);
}

void goToSleep() {
  ADCSRA = 0; // отключаем АЦП
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}
