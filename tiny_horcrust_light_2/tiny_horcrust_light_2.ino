
#include <avr/sleep.h>
#include <avr/power.h>
#define F_CPU 8000000  //F_CPU 8000000. This is used by delay.h library


int light = 0;
int lightread;
long heartbeatMillis = 0;

#include <FastLEDsupport.h>  // вкл поддержку FL

#include <microLED.h>
#define TLED_ORDER ORDER_GRB  // порядок цветов
#define TLED_CHIP LED_WS2812  // чип светодиода ленты

#include "tinyLED.h"
tinyLED<4> strip;  // указываем пин (в порядке порта)

#define NUMLEDS 1  // количество светодиодов (для циклов)


void setup() {

  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  pinMode(PB3, INPUT);

  delay(100);  // Delay for 1 second
    strip.setBrightness(200);
    delay(300);
      strip.send(mBlue);
        delay(300);
        strip.send(mRed);
        delay(300);  
}

void loop() {

 
  lightread = map(analogRead(PB3), 0, 1024, 0, 255);


  // if ((lightread > 10) && (lightread < 150)) {
  //   strip.sendRGB(100, 0, 0);
  // }

  // else if ((lightread > 150) && (lightread < 200)) {
  //   strip.sendRGB(0, 100, 0);
  // }

  // else if ((lightread > 230) && (lightread < 240)) {
  //   strip.sendRGB(0, 0, 100);
  // }

  // else if ((lightread > 240)) {
  //   strip.sendRGB(100, 100, 100);
  // }

  // delay(200);
 
  if (millis() - heartbeatMillis >= 30000) {
    //restart this TIMER
    heartbeatMillis = millis();
    light = 0;
  }
 
      if (lightread < 12) {
        light++;
        heartbeatMillis = millis();

        strip.send(mGreen);
        delay(300);
        strip.send(mRed);
        delay(300);  
      }
 
  if (light == 11) {
    strip.clear(1);
    delay(200);

    enterDeepSleep();
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
