#include <avr/wdt.h>
#define DECODE_DISTANCE_WIDTH  // Universal decoder for pulse distance width protocols
//#define DECODE_HASH
#include <IRremote.h>
volatile boolean f_wdt = 1;
const int ledPin = 5;  // Пин, к которому подключен светодиод
unsigned long timing, timing2;
byte caunt, natlamp = 0,brightness;



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
  IrReceiver.begin(3, ENABLE_LED_FEEDBACK);
  delay(100);               // Delay for 1 second
  pinMode(ledPin, OUTPUT);  // кнопка на D12 и GND
  pinMode(4, OUTPUT);       // кнопка на D12 и GND
  pinMode(5, OUTPUT);       // кнопка на D12 и GND
  pinMode(7, OUTPUT);       // кнопка на D12 и GND
  digitalWrite(7, HIGH);
   Serial.begin(115200);
}

void loop() {
  //wdt_reset();
  if (millis() - timing > 100) {
    if (natlamp == 1) {
      digitalWrite(7, LOW);  // turn LED off by turning off sinking transistor
    } else {
      digitalWrite(7, HIGH);  // turn LED on by sinking current to ground
    }
    caunt == 11 ? caunt = 0 : caunt++;
if (caunt==10&natlamp==1)
{
    analogWrite(ledPin, (brightness <2 ? brightness = 0 : brightness--));  // Устанавливаем яркость
}
    timing = millis();
  
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


    for ( brightness = 0; brightness <= 254; brightness++) {
      analogWrite(ledPin, brightness);  // Устанавливаем яркость
      delay(10);                        // Задержка для плавного изменения яркости
    }
  }
  if ((newCode == 1111000002) || (newCode == 16718055)) {


    // Уменьшение яркости светодиода
    // for (brightness = 255; brightness >= 0; brightness--) {
      analogWrite(ledPin, 0);  // Устанавливаем яркость
    //   delay(1000);                      // Задержка для плавного изменения яркости
    // }
  }


  if ((newCode == 1111000005) || (newCode == 16726215)) {
    digitalWrite(4, HIGH);  // turn LED off by turning off sinking transistor
    delay(200);
    digitalWrite(4, LOW);  // turn LED on by sinking current to ground
    newCode = 0;
    natlamp = 1;
    
  }


    if (newCode == 16736925)  {

      for ( brightness = 0; brightness <= 254; brightness++) {
      analogWrite(ledPin, brightness);  // Устанавливаем яркость
      delay(600);                        // Задержка для плавного изменения яркости
      }
      natlamp = 0;
  }

  IrReceiver.resume();
  newCode = 0;
}
}
