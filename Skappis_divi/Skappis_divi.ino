#include <avr/wdt.h>
#define DECODE_DISTANCE_WIDTH  // Universal decoder for pulse distance width protocols
//#define DECODE_HASH

#include <IRremote.h>
#define F_CPU 8000000  //F_CPU 8000000. This is used by delay.h library
volatile boolean f_wdt = 1;
bool door1, door2;
int aloomore;


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
  wdt_enable(WDTO_8S);
  IrReceiver.begin(PB0);
  pinMode(PB4, OUTPUT);  // кнопка на D12 и GND
  digitalWrite(PB4, 0);
    pinMode(PB3, OUTPUT);  // кнопка на D12 и GND
  digitalWrite(PB3, 0);
}

void loop() {
  wdt_reset();

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
  
    if ((newCode == 1111000005) || (newCode == 16726215)) {
    aloomore=aloomore+10;
    }


    IrReceiver.resume();
    newCode = 0;
  }


  if (door1==0&&aloomore>=5) 
{
  door1=1;
 digitalWrite(PB3, 1);
 delay(700);
 digitalWrite(PB3, 0);

}
else if(door1==1&&door2==0&&aloomore>29) 
{
 door2=1;
 digitalWrite(PB4, 1);
 delay(700);
 digitalWrite(PB4, 0);
 aloomore=0;
 door1=0;door2=0;
}
}
