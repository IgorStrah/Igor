
#include <SPI.h>; // Библиотека для работы с шиной SPI
#include <nRF24L01.h>; // Файл конфигурации для библиотеки RF24
#include <RF24.h>; // Библиотека для работы с модулем NRF24L01
#include <IRremote.h>
#define PIN_CE 8 // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 7 // Номер пина Arduino, к которому подключен вывод CSN радиомодуля

RF24 radio(PIN_CE, PIN_CSN); // Создаём объект radio с указанием выводов CE и CSN
uint32_t newCode[2];

void setup() {
  Serial.begin(115200); // Инициализация серийного порта для отладки
  IrReceiver.begin(2);
  radio.begin();  // Инициализация радиомодуля NRF24L01
  radio.setChannel(5); // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate (RF24_1MBPS); // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH); // Выбираем высокую мощность передатчика (-6dBm)
  radio.openWritingPipe (0x7878787878LL); // Открываем трубу с уникальным ID

  delay(2000); // Задержка на инициализацию датчика DHT
}

void loop() {

  if (IrReceiver.decode()) {
    unsigned long irValue = IrReceiver.decodedIRData.decodedRawData;  // Получение значения ИК сигнала

    for (int i = 0; i < 32; i++) {
      // Extract the ith bit from the old code
      uint32_t bit = (irValue >> (31 - i)) & 1;

      // Set the ith bit in the new code
      newCode[0] |= (bit << i);
    }
    if (newCode[0]!=0)
    {
        //  newCode[1]=101;
          Serial.println(newCode[0]);
    }

 if ((newCode[0] == 1111000005) || (newCode[0] == 16726215)) {
       radio.write(newCode,  sizeof(newCode)); // Передаём данные по радиоканалу
 }
 else if ((newCode[0] == 1111000006) || (newCode[0] == 16734885)) {
   radio.write(newCode,  sizeof(newCode)); // Передаём данные по радиоканалу
 }
    IrReceiver.resume();
    newCode[0] = 0;
  }
   
 
  

}