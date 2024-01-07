#include <avr/wdt.h>
#define DECODE_DISTANCE_WIDTH  // Universal decoder for pulse distance width protocols
//#define DECODE_HASH


#include "ATtinySerialOut.hpp"  // TX is at pin 2 - Available as Arduino library "ATtinySerialOut" - Saves up to 700 bytes program memory and 70 bytes

#include <IRremote.h>
#define F_CPU 8000000  //F_CPU 8000000. This is used by delay.h library
volatile boolean f_wdt = 1;
int color;
byte myInts[72];
// ===============================================================
#include <FastLEDsupport.h> // вкл поддержку FL

#include <microLED.h>
#define TLED_ORDER ORDER_GRB  // порядок цветов
#define TLED_CHIP LED_WS2812  // чип светодиода ленты
#include "tinyLED.h"
tinyLED<1> strip;  // указываем пин (в порядке порта)

#define NUMLEDS 72  // количество светодиодов (для циклов)

uint16_t Timer_Fire=0;

void setup() {
  wdt_reset();
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
// Routines to set and clear bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
  Serial.begin(115200);
  Serial.println("Start ");
  wdt_enable(WDTO_8S);
  IrReceiver.begin(PB0, ENABLE_LED_FEEDBACK);
  delay(100);  // Delay for 1 second

  strip.setBrightness(255);
    strip.clear(72);
  
}

void loop() {
  wdt_reset();



  if (Timer_Fire == 1) {
   
    Timer_Fire = 0;
   
       strip.clear(72);
  }
  if (Timer_Fire != 0) {
    fireEffect();
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
       Serial.println(newCode);
      if ((newCode == 1111000001) || (newCode == 16724175)) {
        Timer_Fire=3000;
              IrReceiver.resume();
        newCode = 0;
      }
      if ((newCode == 1111000002) || (newCode == 16718055)) {
        Timer_Fire=0;
        strip.clear(72);
        newCode = 0;
            IrReceiver.resume();
      }
      IrReceiver.resume();
    }

}

void fireEffect() {


   byte flicker = random(160, 255); 

  for (int i = 0; i < 71; i++) {
    myInts[i]=random(140, 254); 
  
  }
  noInterrupts();
  strip.begin();
  for (int i = 0; i < 71; i++) {

    strip.sendRGB(flicker, myInts[i] / 20,myInts[i]);
   
  }
  interrupts();
  delay(random(50, 120));  // Задержка между итерациями эффекта
  

  
  Timer_Fire--;
}
