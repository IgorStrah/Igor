
#include <Tiny4kOLED.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <Wire.h>
#include <OneWire.h>

int DS18S20_Pin = PB1;  //DS18S20 Signal pin on digital 2
//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 2

unsigned long millis1 = 0;  // Счётчик миллисекунд
#define OUTPUT_PIN PB3     // Пин для светодиода (PB1)

uint32_t lastTime = 0;    // Последнее время проверки
uint32_t lastToggle = 0;  // Время последнего инвертирования
uint16_t counter = 0;     // Счётчик секунд
  uint8_t state = 0;  // Состояние выхода (0 или 1)


void setup() {
  // put your setup code here, to run once:
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  // Настраиваем таймер0 для подсчёта миллисекунд
  TCCR0A = (1 << WGM01);               // Режим CTC
  TCCR0B = (1 << CS01) | (1 << CS00);  // Предделитель 64
  OCR0A = 124;                         // Прерывание каждую 1 мс при 8 МГц
  TIMSK = (1 << OCIE0A);               // Разрешаем прерывание по совпадению
  sei();                               // Включаем глобальные прерывания


    DDRB |= (1 << OUTPUT_PIN); // Настраиваем PB4 как выход
    PORTB &= ~(1 << OUTPUT_PIN);


/*
  oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
  oled.clear();
  oled.on();
 

  oled.setFont(FONT8X16P);
  oled.setCursor(0, 1);
  oled.print("MPU6050-OK");
 */
}

ISR(TIM0_COMPA_vect) {
  millis1++;  // Увеличиваем счётчик миллисекунд
}

void loop() {
  float temperature = getTemp();

  // Проверяем вход
  if (temperature > 54 && temperature < 63) 
{
    if (millis1 - lastTime >= 1000) {
      lastTime = millis1;  // Обновляем время последней проверки
      counter++;           // Увеличиваем счётчик секунд

      if (counter <= 65) {
        // Инвертируем состояние PB4 каждую секунду в течение 15 секунд
        state ^= 1;
        if (state) {
          PORTB |= (1 << OUTPUT_PIN);
        } else {
          PORTB &= ~(1 << OUTPUT_PIN);
        }
      } else {
        // Если прошло 15 секунд, выставляем PB4 в 0
        PORTB &= ~(1 << OUTPUT_PIN);
        enterDeepSleep();
      }
    }
  } else {

    counter = 0;
    if (temperature>1)
    { 
      PORTB |= (1 << OUTPUT_PIN);  // Устанавливаем PB4 в 1}
    }
   
    state = 0;
    lastTime = millis1;  // Обновляем время, чтобы избежать лишних инверс
  }
/*
  oled.setFont(FONT8X16P);
  oled.setCursor(0, 1);
  oled.print("T:  ");
  oled.setFont(FONT8X16P);
  oled.setCursor(28, 1);
  oled.print(temperature);

*/

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
