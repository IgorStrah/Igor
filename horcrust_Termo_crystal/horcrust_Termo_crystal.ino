
// дефайны настроек, прописываются перед подключением либы
#define TLED_ORDER ORDER_GRB   // порядок цветов
#define TLED_CHIP LED_WS2812   // чип светодиода ленты
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <Wire.h>
#include <OneWire.h>
#include "tinyLED.h"
tinyLED<1> strip;   
int DS18S20_Pin = PB3;  //DS18S20 Signal pin on digital 2
//Temperature chip i/o
OneWire ds(DS18S20_Pin);       // on digital pin 2
uint32_t lastTime = 0;         // Последнее время проверки
uint32_t lastToggle = 0;       // Время последнего инвертирования
uint16_t counter = 255;          // Счётчик секунд
byte state = 0;                // Состояние выхода (0 или 1)


void setup() {
  // put your setup code here, to run once:
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  // Настраиваем таймер0 для подсчёта миллисекунд
   delay(100);

  state = 0;
}



void loop() {
  float temperature = getTemp();

  // Проверяем вход
  if (temperature > 54 && temperature < 65) {
    if (millis() - lastTime >= 1000) {
      lastTime = millis();  // Обновляем время последней проверки
      if (counter >= 15) {
        counter = counter-5;
       strip.sendRGB(0,counter,0);
      }
      else
      {
      strip.clear(1);
      enterDeepSleep();
        
      }
    }
  } else {
    counter=254;  
    state = 0;

    if (temperature > 1) {
      strip.send(mRed);
    }
    lastTime = millis();  // Обновляем время, чтобы избежать лишних инверс
  }
}


float getTemp() {
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if (!ds.search(addr)) {
    //no more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1000;
  }

  if (addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");
    return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);  // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);  // Read Scratchpad


  for (int i = 0; i < 9; i++) {  // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB);  //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;
}


void enterDeepSleep() {
  // Отключаем ненужные модули для экономии энергии
  power_all_disable();

  // Устанавливаем режим сна "Power-down"
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Включаем режим сна
  sleep_enable();

  // Переходим в сон
  sleep_cpu();

  // После пробуждения (если оно произойдёт) выполнение начнётся отсюда, но для ATtiny85 это не нужно
  sleep_disable();
}
