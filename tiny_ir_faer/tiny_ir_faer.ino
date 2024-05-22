#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "ATtinySerialOut.hpp" // TX is at pin 2 - Available as Arduino library "ATtinySerialOut" - Saves up to 700 bytes program memory and 70 bytes 
#define IR_RECEIVE_PIN    0 // PCINT0

#include "TinyIRReceiver.hpp"

#define LED_COUNT 27  // Количество светодиодов в ленте
#define GROUP_SIZE 4  // Размер группы светодиодов

#if !defined(STR_HELPER)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif

volatile struct TinyIRReceiverCallbackDataStruct sCallbackData;
uint16_t Timer_Fire;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, 3, NEO_GRB + NEO_KHZ800);
volatile boolean f_wdt = 1;
int aCommandn,newcod;
void setup() {
       noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
    Serial.begin(115200);

    // Enables the interrupt generation on change of IR input signal
    if (!initPCIInterruptForTinyReceiver()) {
        Serial.println(F("No interrupt available for pin " STR(IR_RECEIVE_PIN))); // optimized out by the compiler, if not required :-)
    }
  strip.setBrightness(55);
  strip.begin();
  strip.show();   // Инициализация ленты
  strip.clear();  // Инициализация ленты
<<<<<<< HEAD
Serial.println("Start ");
=======

>>>>>>> 959c1c5c140a20c38fce2af7a0788d34919c05f4

}

void loop() {



  if (Timer_Fire == 1) {
    for (int i = 100; i > 0; i--) {
      strip.setBrightness(i);
      strip.show();
      delay(10);
    } 
    Timer_Fire = 0;
    strip.clear();  // Инициализация ленты
  }
  if (Timer_Fire!=0) {
    fireEffect();
  }




if (newcod==1)
{
Serial.println(1);
Serial.println(aCommandn);
//delay(100);
newcod=0;
}

 
}




void handleReceivedTinyIRData(uint8_t aAddress, uint8_t aCommand, uint8_t aFlags)

{
   
     if (aFlags != IRDATA_FLAGS_IS_REPEAT) {
        aCommandn=aCommand;
<<<<<<< HEAD
        Serial.println(1);
        Serial.println(aCommandn);
       // newcod=1;
    }

    if (aCommand==12)
=======
       // newcod=1;
    }

    if (aCommand==22)
>>>>>>> 959c1c5c140a20c38fce2af7a0788d34919c05f4
    {
      strip.setBrightness(255);
       Timer_Fire = 9000; 
aCommandn=aCommand;
    newcod=1;
    }

<<<<<<< HEAD
        if (aCommand==24)
=======
        if (aCommand==25)
>>>>>>> 959c1c5c140a20c38fce2af7a0788d34919c05f4
    {
      Timer_Fire=1;

    aCommandn=aCommand;
    newcod=1;
    }

}



void fireEffect() {
  // Реализация эффекта колыхания огня на адресной ленте с разными оттенками для групп по 4 пикселя
  for (int i = 0; i < LED_COUNT; i += GROUP_SIZE) {
    int baseFlicker = random(180, 255);  // Основной оттенок для группы
    for (int j = 0; j < GROUP_SIZE; j++) {
      int flicker = baseFlicker - j * 10;  // Различные оттенки внутри группы
     // strip.setPixelColor(i + j, strip.Color(flicker/20, flicker / 7, flicker/6));
      strip.setPixelColor(i + j, strip.Color(flicker, flicker / 15,flicker));
    }
  }
  strip.show();
  delay(random(20, 100));  // Задержка между итерациями эффекта
  Timer_Fire--;
<<<<<<< HEAD
}
=======
}
>>>>>>> 959c1c5c140a20c38fce2af7a0788d34919c05f4
