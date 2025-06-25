
#include <Tiny4kOLED.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <DFRobot_QMC5883.h>
#include <math.h>
#include "tinyLED.h"
tinyLED<3> strip;  // указываем пин (в порядке порта)
DFRobot_QMC5883 compass(&Wire, /*I2C addr*/ HMC5883L_ADDRESS);
#define ENABLE_FEATURE_1 0
int azimuth = 0;
int lastStableAngle = -999;
unsigned long lastChangeTime = 0;
unsigned long lastCheckTime = 0;

const int ANGLE_TOLERANCE = 15;
const unsigned long STABLE_DURATION = 2000;  // 5 секунд
const unsigned long CHECK_INTERVAL = 1000;   // 1 секунда

char colorSequence[5] = "0000";        // массив + '\0'
const char targetSequence[] = "ABAD";  // РАБОТАЕТ ЗЕРКАЛЬНО!
char colorChar = '0';

// Переменные для шагов и таймеров
bool isStep1 = true;
bool isStep2 = false;
bool first = true;
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


#if ENABLE_FEATURE_1
  oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
  oled.on();
  oled.clear();
#endif


  strip.setBrightness(200);
  strip.send(mBlue);
  // delay(300);
  // strip.send(mRed);
  // delay(300);

  compass.begin();
}

void loop() {
  unsigned long currentTime = millis();
  float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / PI);
  compass.setDeclinationAngle(declinationAngle);
  sVector_t mag = compass.readRaw();
  compass.getHeadingDegrees();


  int azimuth = atan2(mag.XAxis, mag.YAxis) * 180.0 / PI;  // Азимут в градусах

  // Приведение к диапазону 0–360
  if (azimuth < 0) {
    azimuth += 360;
  }
  // Проверка раз в секунду
  if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
    lastCheckTime = currentTime;

    // Проверка, изменился ли угол
    if (abs(azimuth - lastStableAngle) > ANGLE_TOLERANCE) {
      lastStableAngle = azimuth;
      lastChangeTime = currentTime;
      strip.send(mRed);
    }

    // Если угол стабилен 5 секунд
    if (currentTime - lastChangeTime >= STABLE_DURATION) {
      if (isWithinRange(lastStableAngle, 165)) {
        strip.send(mWhite);
        colorChar = 'A';  // Белый
      } else if (isWithinRange(lastStableAngle, 200)) {
        strip.send(mNavy);
        colorChar = 'B';  // Синий
      } else if (isWithinRange(lastStableAngle, 285)) {
        strip.send(mGreen);
        colorChar = 'C';  // Зелёный
      } else if (isWithinRange(lastStableAngle, 250)) {
        strip.send(mPurple);
        colorChar = 'D';  // Голубой
      } else {
        strip.send(mRed);
      }

      if (colorChar != colorSequence[0]) {
        // сдвиг влево и добавление новой буквы
        for (int i = 3; i > 0; i--) {
          colorSequence[i] = colorSequence[i - 1];
        }
        colorSequence[0] = colorChar;

        // проверка последовательности
        if (strcmp(colorSequence, targetSequence) == 0) {
          delay(2000);
          strip.clear(1);
          delay(200);
          strip.clear(1);
          delay(200);
          enterDeepSleep();
        }
      }
    }
  }



#if ENABLE_FEATURE_1
  oled.clear();


  oled.setFont(FONT8X16P);
  oled.setCursor(14, 0);
  oled.print(azimuth);
  oled.setCursor(28, 2);
  oled.print(mag.HeadingDegress);
#endif
}


// Проверка, входит ли угол в интервал ±5 от точки
bool isWithinRange(int value, int center) {
  int delta = abs((value - center + 360) % 360);
  if (delta > 180) delta = 360 - delta;
  return delta <= ANGLE_TOLERANCE;
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