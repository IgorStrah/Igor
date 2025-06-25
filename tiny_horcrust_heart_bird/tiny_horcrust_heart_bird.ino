
#include <avr/sleep.h>
#include <avr/power.h>
#define F_CPU 8000000  //F_CPU 8000000. This is used by delay.h library
#define DECODE_DISTANCE_WIDTH  // Universal decoder for pulse distance width protocols
//#define DECODE_DISTANCE_WIDTH
//#define EXCLUDE_EXOTIC_PROTOCOLS
//#define DISABLE_PARITY_CHECKS
#include <IRremote.h>

int light=0;
long heartbeatMillis = 0;

#include <FastLEDsupport.h> // вкл поддержку FL

#include <microLED.h>
#define TLED_ORDER ORDER_GRB  // порядок цветов
#define TLED_CHIP LED_WS2812  // чип светодиода ленты

#include "tinyLED.h"
tinyLED<3> strip;  // указываем пин (в порядке порта)

#define NUMLEDS 1  // количество светодиодов (для циклов)


void setup() {

  
  IrReceiver.begin(PB4,ENABLE_LED_FEEDBACK);
    delay(100);  // Delay for 1 second


strip.send(mRed);
  delay(1000);
  
  
}

void loop() {



    if (millis() - heartbeatMillis >= 30000) {
      //restart this TIMER
      heartbeatMillis = millis();
      light=0;
  }


  if (IrReceiver.decode()) {
    unsigned long irValue = IrReceiver.decodedIRData.decodedRawData;  // Получение значения ИК сигнала

    // New LSB first 32-bit IR data code
    uint32_t newCode = 0;

    for (int i = 0; i < 32; i++) {
      // Extract the ith bit from the old code
      uint32_t bit = (irValue >> (31 - i)) & 1;

      // Set the ith bit in the new code
      newCode |= (bit << i);
    }

    if ((newCode == 1111000001) || (newCode == 16724175)) {
    
light++;
   heartbeatMillis = millis();

strip.send(mRed);
delay(300);
strip.send(mBlue);
  delay(300);
strip.send(mRed);
    }
   
    IrReceiver.resume();
    newCode = 0;
  }


if (light==6)
{
  strip.send(mBlue);
  delay(300);
strip.send(mBlue);

  delay(380000);
strip.clear(1);
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
