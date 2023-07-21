#include <Tiny4kOLED.h>
#define DECODE_DISTANCE_WIDTH  // Universal decoder for pulse distance width protocols
//#define DECODE_HASH
#include <Arduino.h>

    



#include <IRremote.h>
#define F_CPU 8000000  //F_CPU 8000000. This is used by delay.h library

decode_results results;

void setup()
{
  DDRB  |=  (1<<1)|(1<<4); // Устанавливаем пин D13 в режим OUTPUT
  noInterrupts();
  CLKPR = 0x80; // enable clock prescale change
  CLKPR = 0;    // no prescale
  interrupts();
 IrReceiver.begin(PB3, ENABLE_LED_FEEDBACK);
      FastLED.addLeds<WS2813, DATA_PIN, RGB>(leds, NUM_LEDS);
   
    oled.begin(128, 64, sizeof(tiny4koled_init_128x32r), tiny4koled_init_128x32br);
  oled.clear();

 delay(1000); // Delay for 1 second
}

void loop() {
	static uint8_t hue = 0;
       //PORTB |= (1 << 2);
       // delay (1000);
       // PORTB &= ~ (1 << 2);
       // delay (1000);

  
if (IrReceiver.decode()) {

    unsigned long irValue = IrReceiver.decodedIRData.decodedRawData;  // Получение значения ИК сигнала
  
    delay(100);


    // New LSB first 32-bit IR data code
  uint32_t newCode = 0;

    for (int i = 0; i < 32; i++) {
      // Extract the ith bit from the old code
      uint32_t  bit = (irValue >> (31 - i)) & 1;

      // Set the ith bit in the new code
      newCode |= (bit << i);
    }
    oled.clear();
 
  oled.setFontX2(FONT6X8P);
  oled.setCursor(0, 3);
  oled.print(newCode);

      if ((newCode== 16726215) || (newCode == 1111000004|| (newCode == 1111000001))) {
         oled.setFontX2(FONT6X8P);
  oled.setCursor(0, 1);
  oled.print("!!!!!!" );

  
  
 }
 IrReceiver.resume();
}

}
