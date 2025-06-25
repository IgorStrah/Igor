#include <microLED.h>
#include <avr/sleep.h>
#include <avr/power.h>
// Настройки LED
#define LED_PIN PB3  // PB3
#define NUM_LEDS 2
microLED<NUM_LEDS, LED_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> led;

// История температуры
#define TEMP_HISTORY_SIZE 5
float tempHistory[TEMP_HISTORY_SIZE] = { 0 };
uint8_t tempIndex = 0;
bool bufferFilled = false;
uint8_t consecutiveDropCount = 0;

#define DROP_THRESHOLD_PERCENT 6.0
#define RISE_THRESHOLD_PERCENT 12.0

void setup() {
  noInterrupts();
  CLKPR = 0x80;  // enable prescaler change
  CLKPR = 0x00;  // no prescaler
  interrupts();

  led.setBrightness(120);
  led.send(mBlue);
  led.show();
  delay(100);
  led.send(mBlue);
  led.show();
  delay(1100);
  led.send(mRed);
  led.show();
 

  delay(100);
  ADCSRA &= ~(_BV(ADATE) | _BV(ADIE));
  ADCSRA |= _BV(ADEN);
  ADMUX = 0xF | _BV(REFS1);
  delay(100);
  getADC();
}

void loop() {
  float tempNow = chipTemp(getSmoothedADCTemp());

  // Обновляем историю
  tempHistory[tempIndex] = tempNow;
  tempIndex = (tempIndex + 1) % TEMP_HISTORY_SIZE;
  if (tempIndex == 0) bufferFilled = true;

  if (bufferFilled) {
    float maxTemp = tempHistory[0];
    float minTemp = tempHistory[0];
    for (uint8_t i = 1; i < TEMP_HISTORY_SIZE; i++) {
      if (tempHistory[i] > maxTemp) maxTemp = tempHistory[i];
      if (tempHistory[i] < minTemp) minTemp = tempHistory[i];
    }

    float risePercent = ((tempNow - minTemp) / minTemp) * 100.0;
    float dropPercent = ((maxTemp - tempNow) / maxTemp) * 100.0;

    if (risePercent >= RISE_THRESHOLD_PERCENT) {
      led.fill(mOrange);
      led.show();
      delay(500);
      led.fill(mRed);
      led.show();
    }

    if (dropPercent >= DROP_THRESHOLD_PERCENT) {
      led.fill(mBlue);
      led.show();
      delay(500);
      led.fill(mRed);
      led.show();
      consecutiveDropCount++;
      if (consecutiveDropCount >= 5) {
        handlePersistentDrop();  // тревога
        consecutiveDropCount = 0;
      }
    } else {
      consecutiveDropCount = 0;
    }
  }

  delay(1000);
}


// Тревожное срабатывание: мигаем синим 5 раз
void handlePersistentDrop() {

  led.fill(mMagenta);
  led.show();
  delay(1500);
  led.clear();  // Погасить все светодиоды (установить в чёрный)
  led.show();
  delay(100);
  enterDeepSleep();
}

// Сглаженное измерение температуры
float getSmoothedADCTemp() {
  ADCSRA |= _BV(ADEN);
  ADMUX = 0xF | _BV(REFS1);
  delay(10);
  float avg = (float)getADC();
  for (int i = 2; i < 500; i++) {
    avg += ((float)getADC() - avg) / i;
  }
  ADCSRA &= ~(_BV(ADEN));
  return avg;
}

// Калибровка под кристалл
float chipTemp(float raw) {
  const float offset = 272.9;  // нужно откалибровать под конкретную ATtiny85
  const float coeff = 1.075;
  return (raw - offset) / coeff;
}

// Одиночное измерение
int getADC() {
  ADCSRA |= _BV(ADSC);
  while (ADCSRA & _BV(ADSC))
    ;
  return ADC;
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
