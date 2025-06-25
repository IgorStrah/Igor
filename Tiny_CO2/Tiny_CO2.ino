
#include <TinyWireM.h>
//#include <Tiny4kOLED.h>
#include <DFRobot_SCD4X.h>
#include <avr/sleep.h>
#include <avr/power.h>
// Используем TinyWireM для I2C
#include <avr/io.h>
DFRobot_SCD4X SCD4X(&Wire, /*i2cAddr = */ SCD4X_I2C_ADDR);

#define STRIP_PIN PB3  // пин ленты
#define NUMLEDS 2      // кол-во светодиодов

#define COLOR_DEBTH 3
#include <microLED.h>  // подключаем библу
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;

int num;
void setup() {
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();

  strip.setBrightness(250);
  strip.fill(mWheel8(200));
  strip.show();

  Wire.begin();
  // OLED инициализация
  // oled.begin();
  // oled.clear();
  // oled.on();
  // oled.setFont(FONT6X8);
  // oled.setCursor(0, 0);
  // oled.print(F("Init SCD4X..."));


  SCD4X.begin();

  SCD4X.setTempComp(4.0);

  SCD4X.setSensorAltitude(540);

  SCD4X.enablePeriodMeasure(SCD4X_START_PERIODIC_MEASURE);
  num=0;

  // delay(1000);
}

void loop() {

  if (SCD4X.getDataReadyStatus()) {
    DFRobot_SCD4X::sSensorMeasurement_t data;
    SCD4X.readMeasurement(&data);

    if (data.CO2ppm < 10000) {
      strip.fill(mRed);
      strip.show();
      num=0;
    } else if (data.CO2ppm > 15000) { 
      num=num+1; 
      strip.fill(mAqua);
      strip.show();
    }

    }
     if (num>4)
    {
 
      strip.fill(mNavy);
      strip.show();
   SCD4X.enablePeriodMeasure(SCD4X_STOP_PERIODIC_MEASURE);
 
  SCD4X.setSleepMode(SCD4X_POWER_DOWN);
        delay(1000);
          strip.clear();  // Погасить все светодиоды (установить в чёрный)
    strip.show();
    delay(100);
    enterDeepSleep();
  }

    

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

