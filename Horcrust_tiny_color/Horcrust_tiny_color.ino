#include <TinyWireM.h>
#include <SoftwareSerial.h>

// === Настройки ===
#define APDS9960_ADDR 0x39
#define COLOR_REG 0x94
//R:130 G:1331 B:3737
#define MATCH_RED   200
#define MATCH_GREEN 2000
#define MATCH_BLUE  5500
#define TOLERANCE   1500

#define OUTPUT_PIN 1 // PB4 (pin 3 on chip)

// mySerial по PB3 (pin 2) как TX
SoftwareSerial mySerial(-1, 4); //  
void setup() {
  noInterrupts();
  CLKPR = 0x80;
  CLKPR = 0x00;
  interrupts();

  TinyWireM.begin();
  mySerial.begin(9600);
  pinMode(OUTPUT_PIN, OUTPUT);
  mySerial.println( "Start") ;
  initAPDS9960();

  mySerial.println("Init OK");
}

void loop() {
  uint16_t c, r, g, b;
  readColor(c, r, g, b);

  mySerial.print(F("R:")); mySerial.print(r);
  mySerial.print(F(" G:")); mySerial.print(g);
  mySerial.print(F(" B:")); mySerial.println(b);

  if (isMatch(r, g, b)) {
  
    mySerial.print(F(" MATCH"));
    delay(100);
      digitalWrite(OUTPUT_PIN, HIGH);
    delay(1000);
  } else {
    digitalWrite(OUTPUT_PIN, LOW);
  }

  delay(300); // Считывание раз в 300 мс
}

// === Функции APDS9960 ===

void initAPDS9960() {
  writeRegister(0x80, 0x03); // ENABLE: Power + ALS
  writeRegister(0x81, 0xDB); // ATIME: ~100ms
  writeRegister(0x8F, 0x02); // CONTROL: Gain 4x
}

void writeRegister(uint8_t reg, uint8_t val) {
  TinyWireM.beginTransmission(APDS9960_ADDR);
  TinyWireM.write(reg);
  TinyWireM.write(val);
  TinyWireM.endTransmission();
}

uint16_t readWord(uint8_t reg) {
  TinyWireM.beginTransmission(APDS9960_ADDR);
  TinyWireM.write(reg);
  TinyWireM.endTransmission(false);
  TinyWireM.requestFrom(APDS9960_ADDR, 2);
  uint8_t lsb = TinyWireM.read();
  uint8_t msb = TinyWireM.read();
  return (msb << 8) | lsb;
}

void readColor(uint16_t &c, uint16_t &r, uint16_t &g, uint16_t &b) {
  c = readWord(COLOR_REG);
  r = readWord(COLOR_REG + 2);
  g = readWord(COLOR_REG + 4);
  b = readWord(COLOR_REG + 6);
}

// === Сравнение с эталоном ===

bool isMatch(uint16_t r, uint16_t g, uint16_t b) {
  return (abs(r - MATCH_RED)   < TOLERANCE) &&
         (abs(g - MATCH_GREEN) < TOLERANCE) &&
         (abs(b - MATCH_BLUE)  < TOLERANCE);
}
