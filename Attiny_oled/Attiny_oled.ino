
#include <Tiny4kOLED.h>
// ============================================================================
#include <Wire.h>


const int ImputPin = A3;  // Пин для активации


void setup() {
  // put your setup code here, to run once:
noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  // This example is for a 128x64 screen

  oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
  oled.on();
  oled.clear();  

 pinMode(ImputPin, INPUT);
  
}

void loop() {

  oled.clear();
  oled.setFont(FONT8X16P);
  oled.setCursor(0, 1);
  oled.print("ax_g:");
  oled.setFont(FONT8X16P);
  oled.setCursor(40, 1);
  oled.print(analogRead(ImputPin) );
  delay(1000); // Небольшая задержка для стабильности
}




