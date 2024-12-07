#include <avr/wdt.h>
#define DECODE_DISTANCE_WIDTH  // Universal decoder for pulse distance width protocols
//#define DECODE_HASH
#include <IRremote.h>
volatile boolean f_wdt = 1;
unsigned long timing;  // Переменная для хранения точки отсчета
bool door1, door2;
int aloomore;
int clocker;
void setup() {
  wdt_reset();
// Routines to set and clear bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
  // wdt_enable(WDTO_8S);
  IrReceiver.begin(2, ENABLE_LED_FEEDBACK);
  delay(100);          // Delay for 1 second
  pinMode(3, OUTPUT);  // кнопка на D12 и GND
  pinMode(4, OUTPUT);  // кнопка на D12 и GND
  pinMode(5, OUTPUT);  // кнопка на D12 и GND
  pinMode(6, OUTPUT);  // кнопка на D12 и GND
  digitalWrite(3, 0);
}

void loop() {


    if (millis() - timing > 1000) {  // Вместо 10000 подставьте нужное вам значение паузы
      timing = millis();
  if (door1==0&&aloomore>=5) 
  {
    door1=1;
  digitalWrite(6, 1);
  delay(300);
  digitalWrite(6, 0);
  aloomore=0;
  }
  else if(door1==1&&door2==0&&aloomore>9) 
  {
  door2=1;
  digitalWrite(4, 1);
  delay(300);
  digitalWrite(4, 0);
  aloomore=0;
  }

          aloomore = (aloomore >= 0) ? aloomore-1 : 0;

    }
    //wdt_reset();

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
      if ((newCode == 16736925) || (newCode == 16736925)) {
        door1=0;door2=0;
        digitalWrite(5, 1);
        digitalWrite(3, 1);
        delay(200);
        digitalWrite(5, 0);
        digitalWrite(3, 0);
      }
      if ((newCode == 1111000005) || (newCode == 16726215)) 
      {
      aloomore=aloomore+10;

      }



      IrReceiver.resume();
      newCode = 0;
    }
  
}
