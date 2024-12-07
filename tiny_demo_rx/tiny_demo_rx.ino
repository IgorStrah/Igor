
#define STRIP_PIN 3  // пин ленты
#define NUMLEDS 22   // кол-во светодиодов
#include <Gyver433.h>

Gyver433_RX<2, 20> rx;
#define COLOR_DEBTH 3

#include <microLED.h>  // подключаем библу
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;
int lednumer, clocker;
void setup() {
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);            // wait for a second
  digitalWrite(4, LOW);   // turn the LED off by making the voltage LOW
  strip.setBrightness(60);
  strip.clear();
  attachInterrupt(0, isr, CHANGE);
}

// тикер вызывается в прерывании
void isr() {
  rx.tickISR();
}

void loop() {
lednumer=0;
  if (rx.gotData()) {  // если больше 0
    lednumer  = map(rx.getRSSI(), 0, 100, 0, 20);
  

  strip.clear();
    strip.fill(0, lednumer, mWheel8(100));   // заливаем ~зелёным с 3 по 6:
    strip.show();
    clocker=0;   
  
  }
  if (clocker==100)
  {
    strip.clear();
    strip.show();
    digitalWrite(4, LOW); 
  }
  clocker++;
  delay(10);
  
}
