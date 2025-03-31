#include <Adafruit_NeoPixel.h>
#include <Tiny4kOLED.h>
// ============================================================================
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h>
#define LED_PIN PB4
#define NUM_LEDS 2
#define SENSOR_PIN PB3

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
#define ENABLE_FEATURE_1 0

unsigned long lastUpdateTime = 0;      // Таймер обновления раз в 100 мс
unsigned long animationStartTime = 0;  // Когда началась 30-секундная анимация
bool animationActive = false;          // Флаг анимации
int animationStep = 0;   
bool powerOff = false;                 // Флаг выключения контроллера


void setup() {
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  strip.begin();
  strip.show();
   pinMode(SENSOR_PIN, INPUT);
   #if ENABLE_FEATURE_1
    Wire.begin();
     oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
  oled.off();
  oled.clear();
 oled.on();


 #endif


}

void loop() {
    if (powerOff) {
    powerDown();  // Выключаем ATtiny85
  }
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= 100) {  // Обновление каждые 100 мс
    lastUpdateTime = currentTime;
    updateLeds();
   #if ENABLE_FEATURE_1
      oled.clear();
  oled.setFont(FONT8X16P);
  oled.setCursor(0, 1);
  oled.print("data:");
  oled.setFont(FONT8X16P);
  oled.setCursor(28, 1);
  oled.print(digitalRead(analogRead(SENSOR_PIN)) );
#endif
  }
}

void updateLeds() {
  int sensorValue = analogRead(SENSOR_PIN);

  if (sensorValue < 320) {  
    // Просто красный, сбрасываем анимацию
    animationActive = false;
    setColor(255, 0, 0);
  } 
  else if (sensorValue > 400) {  
    // Запускаем 30-секундную анимацию
    if (!animationActive) {
      animationActive = true;
      animationStartTime = millis();
    }
  }

  if (animationActive) {
    if (sensorValue < 300) {  
      // Если во время анимации значение упало ниже 200 — сбрасываем
      animationActive = false;
      setColor(255, 0, 0);
      return;
    }

    runAnimation();
    
    if (millis() - animationStartTime >= 30000) {
      animationActive = false;
      setColor(0, 0, 0);  // Полностью выключаем светодиоды
      delay(500);         // Небольшая задержка перед отключением
      powerOff = true;    // Флаг для выключения контроллера
    }
  }
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

void runAnimation() {
  unsigned long elapsed = millis() - animationStartTime;
  int phase = elapsed / 5000;  // Разбиваем 30 сек на 6 фаз по 5 сек

  switch (phase) {
    case 0: setColor(0, 0, 20); break;   // Тусклый синий
    case 1: setColor(0, 0, 255); break;  // Яркий синий
    case 2: { // Переход синий → зеленый
      float progress = (elapsed % 5000) / 5000.0;
      setColor(0, (int)(progress * 255), 255 - (int)(progress * 255));
      break;
    }
    case 3: setColor(0, 255, 0); break;  // Ярко-зеленый
    case 4: { // Постепенное затухание
      float progress = (elapsed % 5000) / 5000.0;
      setColor(0, 255 - (int)(progress * 255), 0);
      break;
    }
    case 5: setColor(0, 0, 0); break;    // Выключение
  }
}

// Функция отключения ATtiny85
void powerDown() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();  // Отключаем прерывания
  sleep_enable();
  sei();  // Включаем прерывания
  sleep_cpu();  // Уходим в сон

  // Контроллер проснётся только при аппаратном сбросе (например, по кнопке)
}