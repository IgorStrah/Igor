#define PIN 13        // пин DI
#define NUM_LEDS 46  // число диодов
#include "Adafruit_NeoPixel.h"
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  strip.begin();
  strip.setBrightness(150);    // яркость, от 0 до 255
  strip.clear();                          // очистить
  strip.show();                           // отправить на ленту
}
void loop() {
  // заливаем трёмя цветами плавно
  for (int i = 0; i < 144; i++ ) {   // от 0 до первой трети
    strip.setPixelColor(2, i,0,0);     // залить красным
    strip.show();                         // отправить на ленту
    delay(1);
  }
  for (int i = 144; i > 1; i-- ) {   // от 0 до первой трети
    strip.setPixelColor(2, i,0,0);     // залить красным
    strip.show();                         // отправить на ленту
    delay(1);
  }
for (int i = 0; i < 255; i++ ) {   // от 0 до первой трети
    strip.setPixelColor(5, 0xff0000);     // залить красным
    strip.show();                         // отправить на ленту
    delay(11);
  }


for (int i = 0; i < 255; i++ ) {   // от 0 до первой трети
    strip.setPixelColor(12, 0xff0000);     // залить красным
    strip.show();                         // отправить на ленту
    delay(11);
  }


for (int i = 0; i < 255; i++ ) {   // от 0 до первой трети
    strip.setPixelColor(32, 0xff0000);     // залить красным
    strip.show();                         // отправить на ленту
    delay(11);
  }


for (int i = 0; i < 255; i++ ) {   // от 0 до первой трети
    strip.setPixelColor(22, 0xff0000);     // залить красным
    strip.show();                         // отправить на ленту
    delay(11);
  }


for (int i = 0; i < 255; i++ ) {   // от 0 до первой трети
    strip.setPixelColor(16, 0xff0000);     // залить красным
    strip.show();                         // отправить на ленту
    delay(11);
  }

  // заливаем чёрным
  for (int i = 0; i < NUM_LEDS; i++ ) {   // всю ленту
    strip.setPixelColor(i, 0x000000);     // залить чёрным
    strip.show();                         // отправить на ленту
    delay(10);
  }

  
  for (int i = NUM_LEDS ; i < NUM_LEDS; i++ ) {   // от 1/3 до 2/3
    strip.setPixelColor(i, 0x00ff00);     // залить зелёным
    strip.show();                         // отправить на ленту
    delay(200);
  }
  
  for (int i = NUM_LEDS ; i < NUM_LEDS; i++ ) {   // от 2/3 до конца
    strip.setPixelColor(i, 0x0000ff);     // залить синим
    strip.show();                         // отправить на ленту
    delay(200);
  }
  delay(1000);
  // заливаем белым
  for (int i = 0; i < NUM_LEDS; i++ ) {   // всю ленту
    strip.setPixelColor(i, 0xffffff);     // залить белым
    strip.show();                         // отправить на ленту
    delay(230);
  }
  delay(1000);
  // заливаем чёрным
  for (int i = 0; i < NUM_LEDS; i++ ) {   // всю ленту
    strip.setPixelColor(i, 0x000000);     // залить чёрным
    strip.show();                         // отправить на ленту
    delay(10);
  }
  delay(1000);
 
}
