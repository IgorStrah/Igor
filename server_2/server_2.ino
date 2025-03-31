//#define DEBUG



//--------------------- НАСТРОЙКИ ----------------------
#define CH_NUM 0x95  // номер канала (должен совпадать с передатчиком)
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
#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#define EEPROM_ADDR 0x50  // Адрес I2C для 24C65, обычно это 0x50 (для адреса 0xA0)
#define MAX_WORD_LENGTH 50
RF24 radio(9, 8);  // "создать" модуль на пинах 9 и 10 для НАНО/УНО
//RF24 radio(9, 53); // для МЕГИ
//--------------------- БИБЛИОТЕКИ ----------------------

//--------------------- ПЕРЕМЕННЫЕ ----------------------
byte pipeNo;
byte address[][6] = { "1Node", "2Node", "3Node", "4Node", "5Node", "6Node" };  // возможные номера труб

// Список слов
const char* words[] = { "hell", "god", "lock" };
const int wordCount = sizeof(words) / sizeof(words[0]);

// Переменные для логики игры
char targetWord[MAX_WORD_LENGTH] = "";
int currentLetterIndex = 0, currentLetterClock = 0;
bool gameActive = false;
unsigned long lastReadTime = 0;

byte tempclocll;
int telemetry[2];      // массив данных телеметрии (то что шлём на передатчик)
//--------------------- ПЕРЕМЕННЫЕ ----------------------
int x;
void setup() {
  Wire.begin();
  Serial.begin(115200);
  radioSetup();
  Serial.print("Старт ");
}

void loop() {

    // Проверяем правильность буквы и задержку
    if ( millis() - lastReadTime > 15000) {
      lastReadTime = millis();  // Обновляем таймер
    Serial.println("✅ сенд труба2 буква!");
    tempclocll++;
     char message[32];// = "Data from server" ;
    sprintf(message, "Data from server %d", tempclocll);
    radio.stopListening();  // Останавливаем прослушивание
    delay(5);
    radio.write(&message, sizeof(message));  // Отправляем данные на трубу 2
    delay(5);
    radio.startListening();  // Возвращаемся к прослушиванию


    }

  // Читаем команду для выбора слова
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == '1') {
      startNewGame(0);  // Запуск первого слова
    } else if (cmd == '2') {
      startNewGame(1);  // Запуск второго слова
    } else if (cmd == '3') {
      startNewGame(2);  // Запуск третьего слова
    }
  }


  if (radio.available(&pipeNo)) {  // слушаем эфир
    char receivedUID[16] = "";
    telemetry[0] = 0;
    telemetry[1] = 0;
    radio.read(&receivedUID, sizeof(receivedUID));
    String receivedLetter = searchInEEPROM(receivedUID);

#ifdef DEBUG
        Serial.print("Получено с pipe ");
        Serial.print(pipeNo); // Выводим номер канала
    Serial.print("📤 UID  серверу: ");
    Serial.println(receivedUID);
    if (receivedLetter != '\0') {
      Serial.print("🔎 Найденная буква: ");
      Serial.println(receivedLetter);
      Serial.print("🔎 wait  буква: ");
      Serial.println(targetWord[currentLetterIndex]);
    }
#endif



      if (receivedLetter.charAt(0) == targetWord[currentLetterIndex]) {
        #ifdef DEBUG     
           Serial.println("✅ Верная буква!");
        #endif
        currentLetterClock++;
        telemetry[0] = 15+currentLetterClock*2;
        telemetry[1] = 1122;

        if (currentLetterClock == 10) {
          currentLetterClock = 0;
          currentLetterIndex++;
        }
        if (currentLetterIndex >= strlen(targetWord)) {
          Serial.println("🎉 Задание выполнено!");
          gameActive = false;
        }
      } else {
        currentLetterClock = 0;
           telemetry[0] = 1;
        telemetry[1] = 1;
#ifdef DEBUG
        Serial.println("❌ Ошибка, ждём правильную букву!");
#endif
      }
  //  }


    // отправляем пакет телеметрии
    radio.writeAckPayload(pipeNo, &telemetry, sizeof(telemetry));
     radio.flush_rx();
  }

}

// Функция для поиска UID в памяти 24C64 и возврата соответствующей буквы
String searchInEEPROM(const char* uid) {
  char uidBuffer[15];
  char letterBuffer[2];
  int pageCount = 45;  // Количество страниц для поиска (каждая страница по 32 байта)

  for (int page = 0; page < pageCount; page++) {
    int baseAddr = page * 32;  // Каждый элемент UID + буква занимает 17 байт
    // Читаем данные с EEPROM
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write((baseAddr >> 8) & 0xFF);  // Старший байт адреса
    Wire.write(baseAddr & 0xFF);         // Младший байт адреса
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_ADDR, 17);  // Запрашиваем 17 байт (UID + буква)

    // Читаем UID
    for (int i = 0; i < 15; i++) {
      uidBuffer[i] = Wire.read();
    }

    // Читаем букву
    for (int i = 0; i < 2; i++) {
      letterBuffer[i] = Wire.read();
    }

    // Сравниваем считанный UID с переданным
    if (strncmp(uid, uidBuffer, 15) == 0) {
      return String(letterBuffer);  // Возвращаем букву, если UID совпал
    }
  }

  return "";  // Если не нашли совпадение
}



// Функция запуска игры по внешнему событию
void startNewGame(int wordIndex) {
  if (wordIndex < 0 || wordIndex >= wordCount) return;

  strcpy(targetWord, words[wordIndex]);  // Выбираем слово
  currentLetterIndex = 0;                // Сбрасываем индекс
  gameActive = true;

  Serial.print("🎯 Загадано слово: ");
  Serial.println(targetWord);
}


void radioSetup() {                      // настройка радио
  radio.begin();                         // активировать модуль
  radio.setAutoAck(1);                   // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);               // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();              // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);              // размер пакета, байт
  radio.openReadingPipe(1, address[0]);  // хотим слушать трубу 0
  radio.openWritingPipe(address[1]);   // хотим писать трубу 1
  radio.setChannel(CH_NUM);              // выбираем канал (в котором нет шумов!)
  radio.setPALevel(SIG_POWER);           // уровень мощности передатчика
  radio.setDataRate(SIG_SPEED);          // скорость обмена
  // должна быть одинакова на приёмнике и передатчике!
  // при самой низкой скорости имеем самую высокую чувствительность и дальность!!
  radio.powerUp();         // начать работу
  radio.startListening();  // начинаем слушать эфир, мы приёмный модуль
}
