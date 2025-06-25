#include <TinyWireM.h>              // Библиотека для I2C на ATtiny85
#include <Tiny4kOLED.h>             // Библиотека для OLED дисплея

// Адреса устройств на шине I2C
#define HMC5883L_ADDR 0x1E         // Адрес GY271 (HMC5883L)
#define OLED_ADDR 0x3C              // Адрес OLED дисплея

// Регистры HMC5883L
#define HMC5883L_REG_CONFIG_A 0x00
#define HMC5883L_REG_CONFIG_B 0x01
#define HMC5883L_REG_MODE 0x02
#define HMC5883L_REG_DATA 0x03

void setup() {
      noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  //IrReceiver.begin(PB4);
  TinyWireM.begin();                // Инициализация I2C
 oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);                  // Инициализация OLED
  oled.clear();                     // Очистка экрана
                 // Включение дисплея
  oled.setCursor(0, 0);
   oled.setFont(FONT8X16P);
  oled.setCursor(64, 0);
  oled.print("test");

  oled.on();        
  delay(1000);
  //Настройка HMC5883L 
  TinyWireM.beginTransmission(HMC5883L_ADDR);
  TinyWireM.write(HMC5883L_REG_CONFIG_A);
  TinyWireM.write(0x78);            // 8-средних значений, 15Hz выходной скорости
  TinyWireM.endTransmission();
  
  TinyWireM.beginTransmission(HMC5883L_ADDR);
  TinyWireM.write(HMC5883L_REG_CONFIG_B);
  TinyWireM.write(0xA0);            // Установка диапазона ±5.0 Гаусс
  TinyWireM.endTransmission();
  
  TinyWireM.beginTransmission(HMC5883L_ADDR);
  TinyWireM.write(HMC5883L_REG_MODE);
  TinyWireM.write(0x00);            // Режим непрерывных измерений
  TinyWireM.endTransmission();
  
  delay(100);                       // Задержка для стабилизации
}

void loop() {
  int16_t x, y, z;
  
//  if (IrReceiver.decode()) {
//     unsigned long irValue = IrReceiver.decodedIRData.decodedRawData;  // Получение значения ИК сигнала

//     // New LSB first 32-bit IR data code
//     uint32_t newCode = 0;

//     for (int i = 0; i < 32; i++) {
//       // Extract the ith bit from the old code
//       uint32_t bit = (irValue >> (31 - i)) & 1;

//       // Set the ith bit in the new code
//       newCode |= (bit << i);
//     }
//   oled.setCursor(64, 2);
//   oled.setFont(FONT8X16P);
//   oled.print("ir: ");
//   oled.print(newCode);


//     IrReceiver.resume();
//     newCode = 0;
//   }


  // Чтение данных с магнитометра
  TinyWireM.beginTransmission(HMC5883L_ADDR);
  TinyWireM.write(HMC5883L_REG_DATA);
  TinyWireM.endTransmission();
  
  TinyWireM.requestFrom(HMC5883L_ADDR, 6);
  if (TinyWireM.available() >= 6) {
    x = (TinyWireM.read() << 8) | TinyWireM.read();
    z = (TinyWireM.read() << 8) | TinyWireM.read();
    y = (TinyWireM.read() << 8) | TinyWireM.read();
  }
  
  // Вывод данных на OLED
  oled.clear();
  oled.setCursor(0, 0);
  oled.setFont(FONT8X16P);
  oled.print("X: ");
  oled.print(x);
  
  oled.setCursor(0, 2);
  oled.setFont(FONT8X16P);
  oled.print("Y: ");
  oled.print(y);
  
  oled.setCursor(64, 0);
  oled.setFont(FONT8X16P);
  oled.print("Z: ");
  oled.print(z);
  
  delay(100); // Задержка между обновлениями



  
}

void normalSize() {


  oled.setFont(FONT8X16P);
  oled.setCursor(64, 0);
  oled.print("Datacute");
}