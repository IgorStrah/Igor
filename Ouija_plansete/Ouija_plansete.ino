

// ЭТО СКЕТЧ ПЕРЕДАТЧИКА!!!

//--------------------- НАСТРОЙКИ ----------------------
#define CH_NUM 0x95  // номер канала (должен совпадать с приёмником)
//--------------------- НАСТРОЙКИ ----------------------

//--------------------- ДЛЯ РАЗРАБОТЧИКОВ -----------------------
// УРОВЕНЬ МОЩНОСТИ ПЕРЕДАТЧИКА
// На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
#define SIG_POWER RF24_PA_HIGH

// СКОРОСТЬ ОБМЕНА
// На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
// должна быть одинакова на приёмнике и передатчике!
// при самой низкой скорости имеем самую высокую чувствительность и дальность!!
// ВНИМАНИЕ!!! enableAckPayload НЕ РАБОТАЕТ НА СКОРОСТИ 250 kbps!
#define SIG_SPEED RF24_1MBPS
//--------------------- ДЛЯ РАЗРАБОТЧИКОВ -----------------------

//--------------------- БИБЛИОТЕКИ ----------------------
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
RF24 radio(8, 7);  // CE, CSN
#define VIBRATION_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);
//--------------------- БИБЛИОТЕКИ ----------------------

//--------------------- ПЕРЕМЕННЫЕ ----------------------
byte address[][6] = { "1Node", "2Node", "3Node", "4Node", "5Node", "6Node" };  // возможные номера труб
unsigned long lastReadTime = 0;

int transmit_data[2];  // массив пересылаемых данных
int vibData[2];        // массив принятых от приёмника данных телеметрии
byte rssi;
int trnsmtd_pack = 1, failed_pack;

unsigned long vibEndTime = 0;
char uidBuffer[16] = "NoCard";  // Буфер для UID (если нет карты)

int vibValue = 0;



//--------------------- ПЕРЕМЕННЫЕ ----------------------

void setup() {
  Serial.begin(115200);  // открываем порт для связи с ПК
  SPI.begin();
  mfrc522.PCD_Init();
  radioSetup();
  pinMode(VIBRATION_PIN, OUTPUT);
  analogWrite(VIBRATION_PIN, 0);
  Serial.println("📡 Клиент RFID + nRF24L01 запущен...");
}

void loop() {


  if (millis() - lastReadTime > 200) {
    lastReadTime = millis();  // Обновляем таймер
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // Читаем UID карты
      memset(uidBuffer, 0, sizeof(uidBuffer));
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        sprintf(uidBuffer + i * 2, "%02X", mfrc522.uid.uidByte[i]);
      }

      Serial.print("🆔 UID считан: ");
      Serial.println(uidBuffer);

      // mfrc522.PICC_HaltA();  // Остановить карту
      mfrc522.PCD_StopCrypto1();


      if (radio.write(&uidBuffer, sizeof(uidBuffer))) {

        if (!radio.available()) {  // если получаем пустой ответ
        } else {
          while (radio.available()) {               // если в ответе что-то есть
            radio.read(&vibData, sizeof(vibData));  // читаем
            // получили забитый данными массив telemetry ответа от приёмника

            vibValue = vibData[0];
            vibEndTime = millis() + vibData[1];
            analogWrite(VIBRATION_PIN, vibValue);
          }
        }
      }
    }
  }
  if (vibValue > 0 && millis() >= vibEndTime) {
    analogWrite(VIBRATION_PIN, 0); 
    vibValue = 0;
  }

  /*
  // забиваем transmit_data данными, для примера
  transmit_data[0] = 10;
  transmit_data[0] = 20;

  // отправка пакета transmit_data
  if (radio.write(&transmit_data, sizeof(transmit_data))) {
    trnsmtd_pack++;
    if (!radio.available()) {   // если получаем пустой ответ
    } else {
      while (radio.available() ) {                    // если в ответе что-то есть
        radio.read(&telemetry, sizeof(telemetry));    // читаем
        // получили забитый данными массив telemetry ответа от приёмника

        Serial.println(telemetry[0]);
      }
    }
  } else {
    failed_pack++;
  }

  if (millis() - RSSI_timer > 1000) {    // таймер RSSI
    // расчёт качества связи (0 - 100%) на основе числа ошибок и числа успешных передач
    rssi = (1 - ((float)failed_pack / trnsmtd_pack)) * 100;

    // сбросить значения
    failed_pack = 0;
    trnsmtd_pack = 0;
    RSSI_timer = millis();
  }
  */
}

void radioSetup() {
  radio.begin();                      // активировать модуль
  radio.setAutoAck(1);                // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);            // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();           // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);           // размер пакета, в байтах
  radio.openWritingPipe(address[0]);  // мы - труба 0, открываем канал для передачи данных
  radio.setChannel(CH_NUM);           // выбираем канал (в котором нет шумов!)
  radio.setPALevel(SIG_POWER);        // уровень мощности передатчика
  radio.setDataRate(SIG_SPEED);       // скорость обмена
  // должна быть одинакова на приёмнике и передатчике!
  // при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp();        // начать работу
  radio.stopListening();  // не слушаем радиоэфир, мы передатчик
}