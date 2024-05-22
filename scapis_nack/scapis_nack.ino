#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#define DECODE_DISTANCE_WIDTH  // Universal decoder for pulse distance width protocols
//#define DECODE_HASH
#include <IRremote.h>
#define F_CPU 8000000  //F_CPU 8000000. This is used by delay.h library
volatile boolean f_wdt = 1;
bool light = 0;
long heartbeatMillis = 0;
uint64_t time;



#define LIGHT_SENSOR_PIN PB4
#define OUTPUT_PIN PB3
#define THRESHOLD 1000 // Пороговое значение изменения освещенности

int previousLightLevel = 0; // Для хранения предыдущего значения освещенности

void setup() {
   wdt_reset();
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
   wdt_enable(WDTO_8S);
  IrReceiver.begin(PB0);
  // Настройка пина фоторезистора как входного
  DDRB &= ~(1 << LIGHT_SENSOR_PIN);
  // Настройка выходного пина
  DDRB |= (1 << OUTPUT_PIN);
  // Инициализация АЦП
  ADMUX = (1 << MUX1); // Выбор канала АЦП (PB4 = A2)
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Включение АЦП и установка делителя частоты
}

int readLightSensor() {
  // Запуск преобразования
  ADCSRA |= (1 << ADSC);
  // Ожидание окончания преобразования
  while (ADCSRA & (1 << ADSC));
  // Возврат результата
  return ADC;
}

void loop() {
  int lightLevel = readLightSensor();
  // Проверка изменения уровня освещенности

  if (IrReceiver.decode()) {
    unsigned long irValue = IrReceiver.decodedIRData.decodedRawData;  // Получение значения ИК сигнала

    // New LSB first 32-bit IR data code
    uint32_t newCode = 0;

    for (int i = 0; i < 32; i++) {
      // Extract the ith bit from the old code
      uint32_t bit = (irValue >> (31 - i)) & 1;

      // Set the ith bit in the new code
      newCode |= (bit << i);
    }
   if ((newCode == 1111000005) || (newCode == 16726215)) {

   if (abs(lightLevel )> THRESHOLD) {
    // Изменение существенно, переключаем выход
    PORTB = (1 << OUTPUT_PIN); // Переключение состояния пина PB3
  _delay_ms(1000); // Задержка для стабилизации чтений
    PORTB = (0 << OUTPUT_PIN); // Переключение состояния пина PB3
  }


}
   IrReceiver.resume();
    newCode = 0;
    }
}
  
 


