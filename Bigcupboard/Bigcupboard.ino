#include <avr/wdt.h>
#include <IRremote.h>
#define F_CPU 8000000  //F_CPU 8000000. This is used by delay.h library
volatile boolean f_wdt = 1;

void setup() {
  wdt_reset();

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
  IrReceiver.begin(PB2, ENABLE_LED_FEEDBACK);
  delay(100);            // Delay for 1 second
  pinMode(PB3, OUTPUT);  //  
  digitalWrite(PB3, 0);
  pinMode(PB4, OUTPUT);  //  
  digitalWrite(PB4, 0);
  pinMode(PB5, OUTPUT);  //  
  digitalWrite(PB3, 0);
  pinMode(PB6, OUTPUT);  //  
  digitalWrite(PB4, 0);


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
    Serial.println(newCode);
    if ((newCode == 1111000005) || (newCode == 16726215)) {
      digitalWrite(PB3, 1);
      delay(200);
      digitalWrite(PB3, 0);
    }
/*
    if ((newCode == 1111000001) || (newCode == 16732845)) {
      digitalWrite(PB5, 1);
      delay(1500);
      digitalWrite(PB5, 0);
    }
*/

    if ((newCode == 16736925) || (newCode == 16736925)) {
      digitalWrite(PB4, 1);
      delay(150);
      digitalWrite(PB4, 0);
    }

     if ((newCode == 16769055) || (newCode == 16769055)) {
      digitalWrite(PB5, 1);
      delay(8900);
      digitalWrite(PB5, 0);
    }

        if ((newCode == 16754775) || (newCode == 16754775)) {
      digitalWrite(PB6, 1);
      delay(8350);
      digitalWrite(PB6, 0);
    }


    IrReceiver.resume();
    newCode = 0;
  }
}
