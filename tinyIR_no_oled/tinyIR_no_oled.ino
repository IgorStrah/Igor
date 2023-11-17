
#define DECODE_DISTANCE_WIDTH  // Universal decoder for pulse distance width protocols
//#define DECODE_HASH

#include <FastLED.h>
#define NUM_LEDS 1
#define DATA_PIN PB4

// Define the array of leds
CRGB leds[NUM_LEDS];
int timerled;
#include <IRremote.h>
#define F_CPU 8000000  //F_CPU 8000000. This is used by delay.h library
unsigned long timing;  // Переменная для хранения точки отсчета
unsigned long sliptimer;  // Переменная для хранения точки отсчета
int timingwatchdog=0;         // Переменная для хранения точки отсчета
int lightoff=0;



#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

// these define cbi and sbi, for as far they are not known yet
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))


void setup_watchdog(int ii) 
{
  // 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
  // 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec

  uint8_t bb;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}


// system wakes up when watchdog is timed out
void system_sleep() 
{
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
  setup_watchdog(6);                   // approximately 8 seconds sleep
 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sei();                               // Enable the Interrupts so the wdt can wake us up

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}





void setup() {
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();

  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(200);
  leds[0] = CRGB::Black;
  FastLED.show();
  IrReceiver.begin(PB3, ENABLE_LED_FEEDBACK);
  delay(100);  // Delay for 1 second

}



void loop() {


  if (millis() - timing > 100) {  // Вместо 10000 подставьте нужное вам значение паузы
    timing = millis();
    
    if (timerled >= 2) {
      leds[0] = CHSV(random8(50, 90), 255, random8(160, 200));
      // Show the leds
      FastLED.show();
      timerled--;
    } else if (timerled == 1) {
      lightoff = 30;
      while (lightoff != 0) {
        leds[0] = CHSV(60, 222, lightoff * 4);
        FastLED.show();
        delay(50);
        lightoff--;
      }
      timerled = 0;
    
    } else {

      leds[0] = CRGB::Black;
      FastLED.show();
   
      if (timerled <= -20) {
        timerled = 0;
      //  system_sleep();
      }
      timerled--;
   
    }
   
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

    if ((newCode == 16724175) || (newCode == 1111000001)) {

      leds[0] = CRGB::Red;
      FastLED.show();
      delay(500);
      // Now turn the LED off, then pause
      leds[0] = CRGB::Black;
      FastLED.show();
      delay(500);
      timerled = 600;
      newCode = 0;
    }


    if ((newCode == 16718055) || (newCode == 1111000005)) {
      timerled = 1;
    }
    IrReceiver.resume();
    newCode = 0;
  }


}
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect)
{
 // nothing here
}
