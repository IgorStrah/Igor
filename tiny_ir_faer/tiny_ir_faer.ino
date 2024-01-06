#include <avr/wdt.h>
#define DECODE_DISTANCE_WIDTH  // Universal decoder for pulse distance width protocols
//#define DECODE_HASH


volatile boolean f_wdt = 1;
bool firestart=0;
int clocker;

#define STRIP_PIN 0     // пин ленты
#define NUMLEDS 27      // кол-во светодиодов

#define COLOR_DEBTH 3
#include <microLED.h>   // подключаем библу
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;
#include <FastLEDsupport.h> // вкл поддержку FL


bool fireEffectActive = false;
uint8_t situation = 0;
uint8_t START = 0;
uint8_t x = 0;
uint8_t BIT = 0;
uint8_t Id = 0;
uint8_t Id_inv = 0;
uint8_t Data = 0;
uint8_t Data_back = 0;
uint8_t Data_inv = 0;
uint8_t Repeat = 0;
uint8_t sended = 0;

uint16_t Time_old = 0;
uint16_t Time = 0;
uint16_t TimeDelta = 0;
uint16_t Timer_Fire;
volatile boolean f_wdt = 1;

DEFINE_GRADIENT_PALETTE( heatmap_gp ) {   // делаем палитру огня
  0,     0,  0,  0,     // black
  128,   255,  0,  0,   // red
  224,   255, 255,  0,  // bright yellow
  255,   255, 255, 255  // full white
};
CRGBPalette16 fire_p = heatmap_gp;
void setup() {
  
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  wdt_reset();
  attachInterrupt(0, IR_Read, FALLING);
  
  pinMode(1,OUTPUT);
  digitalWrite(1,LOW);
// Routines to set and clear bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
  strip.setBrightness(250);
  strip.clear();
   strip.show();  
   delay(100);
     strip.clear();
   strip.show();  
}


void loop() {


if (Timer_Fire==1)
{
  for (int i=255; i>0; i--) {
 strip.setBrightness(i);
 strip.show();  
 delay(10);
 Timer_Fire=0;
 fireEffectActive = false;
}


}
if (fireEffectActive) {
    fireEffect();
      strip.show();
    Timer_Fire--;
    delay(90);
  }


  if(sended == 1) {
    if (Data_back == 48 || Data_back==131) { // Замените на код, который соответствует кнопке на вашем ИК-пульте
    strip.setBrightness(255);
    fireEffectActive = !fireEffectActive; 
    Timer_Fire=6000;   // Включаем/выключаем эффект огня
  }
if (Data_back == 24 || Data_back==75) { // Замените на код, который соответствует кнопке на вашем ИК-пульте
    fireEffectActive = false;
    strip.clear(); 
    strip.show();
  }


  //  DigiUSB.println(Data_back, DEC);
    sended = 0;
  } else {
  delay(10);
  }







}


void fireEffect()
{
  static int count = 0;
  count += 10;
  for (int i = 0; i < 27; i++) {
    count += 2;
   strip.set(i, CRGBtoData(ColorFromPalette(fire_p, inoise8(i * 15, count), 255, LINEARBLEND)));
  }
}

void IR_Read(void) {
  digitalWrite(1,HIGH);
  Time = micros();
  if (Time_old != 0) {
    TimeDelta = Time - Time_old;
    if ((TimeDelta > 12000)&&(TimeDelta < 14000)) {
      START = 1;
      x = 0;
      situation = 1;
      Id = 0;
      Id_inv = 0;
      Data = 0;
      Data_inv = 0;
    } else if ((TimeDelta > 10000)&&(TimeDelta < 12000)) {
      situation = 2; // repeat
    } else if ((TimeDelta > 1500)&&(TimeDelta < 2500)) {
      situation = 3; //"1"
      BIT = 1;
    } else if ((TimeDelta > 1000)&&(TimeDelta < 1500)) {
      situation = 3; //"0"
      BIT = 0;
    } else situation = 5; 
    if (situation == 3) {
      if (x < 8) {
        Id |= BIT;
        if (x < 7) Id <<= 1;  
        x++;
      } else if (x < 16) {
        Id_inv |= BIT;
        if (x < 15) Id_inv <<= 1;
        x++;
      } else if (x < 24) {
        Data |= BIT;
        if (x < 23) Data <<= 1;
        x++;
      } else if (x < 32) {
        Data_inv |= BIT;
        if (x < 31) {
          Data_inv <<= 1;
        } else {
          
          /* DO SOMETHING HERE */
          sended = 1;
          
          Data_back = Data;
          Repeat = 0;
        }
        x++;
      }
    } else if (situation == 2) {
      if(Repeat == 1) {
        
        /* DO SOMETHING HERE */
        /*sended = 1;*/
        
      } else {
        Repeat = 1;
      }
    }
  }
  Time_old = Time;
  digitalWrite(1,LOW);
}



