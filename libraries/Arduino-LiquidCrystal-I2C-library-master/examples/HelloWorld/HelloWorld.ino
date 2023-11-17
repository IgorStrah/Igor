#include <SPI.h>; // Библиотека для работы с шиной SPI
#include <nRF24L01.h>;; // Файл конфигурации для библиотеки RF24
#include <RF24.h>; // Библиотека для работы с модулем NRF24L01

#define PIN_CE 10 // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 9 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля

RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN

#include <Wire.h> // Библиотека для работы с шиной 1-Wire
#include <LiquidCrystal_I2C.h> // Библиотека для работы с ЖКИ
#include <LiquidCrystal_I2C.h>
// Создаём объект lcd для работы с дисплеем
// (I2C_ADDR, En_pin, Rw_pin, Rs_pin ,D4_pin, D5_pin, D6_pin, D7_pin)
LiquidCrystal_I2C lcd(0x3F,20,4);

int DHT_value[2]; // Массив для передачи данных о температуре и влажности


uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
uint8_t note[8]  = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
uint8_t duck[8]  = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
uint8_t retarrow[8] = {	0x1,0x1,0x5,0x9,0x1f,0x8,0x4};

void setup() {


  radio.begin();  // Инициализация радиомодуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe(1, 0x7878787878LL); // Открываем трубу с ID передатчика
  radio.startListening(); // Включаем прослушивание открытой трубы
   lcd.init();                      // initialize the lcd 
  lcd.backlight();
  
  lcd.createChar(0, bell);
  lcd.createChar(1, note);
  lcd.createChar(2, clock);
  lcd.createChar(3, heart);
  lcd.createChar(4, duck);
  lcd.createChar(5, check);
  lcd.createChar(6, cross);
  lcd.createChar(7, retarrow);
  lcd.home();
  
  lcd.print("Start Autopilot..");
  lcd.setCursor(0, 1);
  lcd.print(" i ");
  //lcd.printByte(3);
  lcd.print(" arduinos!");
  delay(5000);
  
}

void loop() {
  if(radio.available()){ // Если по рпдиоканалу поступили данные
    radio.read(&DHT_value, sizeof(DHT_value)); // Чтение данных и запись в массив

    // Выводим принятые данные на ЖКИ по координатам
    lcd.setCursor(0, 0); lcd.print("Temperature=");
    lcd.print(DHT_value[0]); lcd.print("oC");

    lcd.setCursor(0, 1); lcd.print("Humidity=");
    lcd.print(DHT_value[1]); lcd.print("%");
  }
}