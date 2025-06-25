#include <TinyWireM.h>
#include <Tiny4kOLED.h>

void setup() {

  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();

  TinyWireM.begin();

  oled.begin();
  oled.clear();
  oled.on();
  oled.setFont(FONT6X8);
  oled.setCursor(0, 0);
  oled.print(F("I2C Scan..."));

  delay(1000);
  scanI2C();
}

void loop() {
  // ничего не делаем после сканирования
}

void scanI2C() {
  uint8_t found = 0;
  uint8_t row = 1;

  for (uint8_t address = 8; address < 120; address++) {
    TinyWireM.beginTransmission(address);
    uint8_t error = TinyWireM.endTransmission();

    if (error == 0) {
      oled.setCursor(0, 2);
      oled.print(F("Found: 0x"));
      if (address < 16) oled.print(F("0"));
      oled.print(address, HEX);
      found++;
      row++;
      delay(100); // пауза для чтения
    }
  }

  if (found == 0) {
    oled.setCursor(0, 2);
    oled.print(F("No devices :("));
  }
}
