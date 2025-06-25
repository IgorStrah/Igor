  //   noInterrupts();
  // CLKPR = 0x80;  // enable clock prescale change
  // CLKPR = 0;     // no prescale
  // interrupts();
#include <TinyWireM.h>

#include <Adafruit_TCS34725.h>
#include <EEPROM.h>
#include <Tiny4kOLED.h>

// --- Пины ---
#define BUTTON_PIN 1  // PB1
#define OUTPUT_PIN 3  // PB3

// --- EEPROM адреса ---
#define EEPROM_ADDR_R 0
#define EEPROM_ADDR_G 2
#define EEPROM_ADDR_B 4

// --- Датчик цвета ---
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X
);

void setup() {
      noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  // Инициализация пинов
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, LOW);

  // I2C для ATtiny85
  TinyWireM.begin();
  delay(100);
  // OLED инициализация
  oled.begin();
  oled.setFont(FONT6X8);
  oled.clear();
  oled.on();

  oled.setCursor(0, 0);
  oled.print(F("Init sensor..."));
delay(200);
  if (!tcs.begin()) {
    oled.setCursor(0, 2);
    oled.print(F("TCS34725 FAIL"));
    while (1); // Остановка при ошибке
  }

  oled.clear();
  oled.setCursor(0, 0);
  oled.print(F("Ready"));
  delay(1000);
}

void loop() {
  static bool lastButtonState = HIGH;

  // Кнопка
  bool buttonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && buttonState == LOW) {
    // Кнопка нажата — сохранить текущий цвет
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);
    EEPROM.put(EEPROM_ADDR_R, r);
    EEPROM.put(EEPROM_ADDR_G, g);
    EEPROM.put(EEPROM_ADDR_B, b);
  }
  lastButtonState = buttonState;

  // Текущий цвет
  uint16_t r_now, g_now, b_now, c;
  tcs.getRawData(&r_now, &g_now, &b_now, &c);

  // Эталон
  uint16_t r_ref, g_ref, b_ref;
  EEPROM.get(EEPROM_ADDR_R, r_ref);
  EEPROM.get(EEPROM_ADDR_G, g_ref);
  EEPROM.get(EEPROM_ADDR_B, b_ref);

  // Сравнение
  int delta = abs((int)r_now - r_ref) +
              abs((int)g_now - g_ref) +
              abs((int)b_now - b_ref);
  bool match = (delta < 100);

  digitalWrite(OUTPUT_PIN, match ? HIGH : LOW);

  // Вывод на дисплей
  oled.clear();
  oled.setCursor(0, 0);
  oled.print(F("R:")); oled.print(r_now);
  oled.setCursor(0, 1);
  oled.print(F("G:")); oled.print(g_now);
  oled.setCursor(0, 2);
  oled.print(F("B:")); oled.print(b_now);
  oled.setCursor(0, 3);
  oled.print(match ? F("MATCH") : F("NO MATCH"));
 

  delay(300);
}

