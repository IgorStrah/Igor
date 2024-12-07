#include <SPI.h>; // Библиотека для работы с шиной SPI
#include <nRF24L01.h>;; // Файл конфигурации для библиотеки RF24
#include <RF24.h>; // Библиотека для работы с модулем NRF24L01
uint32_t newCode[4];
#define PIN_CE 9 // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 8 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля

RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN

#include <Wire.h> // Библиотека для работы с шиной 1-Wire


void setup() {
   Serial.begin(115200); // Инициализация серийного порта для отладки
  radio.begin();  // Инициализация радиомодуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openReadingPipe(1, 0x7878787878LL); // Открываем трубу с ID передатчика
  radio.startListening(); // Включаем прослушивание открытой трубы
  pinMode(2, OUTPUT); digitalWrite(2, HIGH);
}

void loop() {
  if(radio.available()){ // Если по рпдиоканалу поступили данные
    radio.read(&newCode, sizeof(newCode)); // Чтение данных и запись в массив

 Serial.print(newCode[0]); Serial.println(" ");

        if ((newCode[0] == 1111000005) || (newCode[0] == 16726215)) {
     
  digitalWrite(2, LOW); 
  delay(2500);
  digitalWrite(2, HIGH); 
    } else if ((newCode[0] == 1111000006) || (newCode[0] == 16734885)) {
      
  digitalWrite(2, LOW); 
  delay(2500);
  digitalWrite(2, HIGH); 
      }
   
     
     
  }
}