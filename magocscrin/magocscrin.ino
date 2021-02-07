
#include "GyverBus.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


#define TX_PIN 2    // пин
#define TX_ADDR 1   // наш адрес
#define RX_ADDR1 2   // адрес приёмника
#define NUMPIXELS 8 // Popular NeoPixel ring size
#define PIN        A1  // On Trinket or Gemma, suggest changing this to 1
// пин 4, адрес 1, буфер на 15 байт
GBus<TX_PIN, GBUS_FULL> tx(TX_ADDR, 15);

#include <LiquidCrystal.h>
#include <IRremote.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define BTN_UP   1
#define BTN_DOWN 2
#define BTN_LEFT 3
#define BTN_RIGHT 4
#define BTN_SELECT 5
#define BTN_NONE 10
int RECV_PIN = A3;
IRrecv irrecv(RECV_PIN);
decode_results results;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);



byte Flstart = 0;

int sec = 0;
int clocsec=0;
int pixnum=0;

byte timer = 0;
byte menupos = 0;
byte menupos2 = 0;
int KillMonstr = 0;
int button;
int val;
int transmitted = 0; // 0 wait/ 1 request/ 2 send
int transmittedclk=0;
int RX_ADDR;    // адрес приёмника
int RX_ADDRold;
int error = 0;
int detectButton() {
  int keyAnalog =  analogRead(A0);
  if (keyAnalog < 90) {
    // Значение меньше 100 – нажата кнопка right
    return BTN_RIGHT;
  } else if (keyAnalog < 200) {
    // Значение больше 100 (иначе мы бы вошли в предыдущий блок результата сравнения, но меньше 200 – нажата кнопка UP
    return BTN_UP;
  } else if (keyAnalog < 400) {
    // Значение больше 200, но меньше 400 – нажата кнопка DOWN
    return BTN_DOWN;
  } else if (keyAnalog < 600) {
    // Значение больше 400, но меньше 600 – нажата кнопка LEFT
    return BTN_LEFT;
  } else if (keyAnalog < 800) {
    // Значение больше 600, но меньше 800 – нажата кнопка SELECT
    return BTN_SELECT;
  } else {
    // Все остальные значения (до 1023) будут означать, что нажатий не было
    return BTN_NONE;
  }
}


void clearLine(int line) {
  lcd.setCursor(0, 1);
  lcd.print("                ");
}
void printDisplay(String message) {
  Serial.println(message);
  lcd.setCursor(0, 1);
  lcd.print(message);
  delay(1000);
  clearLine(1);

}
void setup() {

  Serial.begin(115200);
  lcd.begin(16, 2);
   lcd.setCursor(0, 0); lcd.print(">Arcade  Seting");
  irrecv.enableIRIn(); // Start the receiver

  randomSeed(analogRead(A2));
pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
pixels.setBrightness(254); // Set BRIGHTNESS to about 1/5 (max = 255)
}
void loop() {
  int button = detectButton();
  int nDevices = 0;


  
  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    lcd.setCursor(0, 0);
    lcd.print("Guesture");
    lcd.setCursor(0, 1);
    lcd.print("              ");
    lcd.setCursor(0, 1);
    lcd.print(results.value);
    irrecv.resume(); // Receive the next value
    delay(300);
    //lcd.setCursor(0, 0);
    //lcd.print("Guesture");
    //lcd.setCursor(0, 1);
    //lcd.print("              ");
    //lcd.setCursor(0, 0);
      }

}
