#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#define MAX_WORD_LENGTH 10
#define EEPROM_ADDR 0x50  // Адрес I2C для 24C65, обычно это 0x50 (для адреса 0xA0)
RF24 radio(9, 8);         // CE, CSN
const byte serverAddress[6] = "Server";
const byte clientAddress[6] = "Client";


// Список слов
const char* words[] = { "hell", "god", "lock" };
const int wordCount = sizeof(words) / sizeof(words[0]);

// Переменные для логики игры
char targetWord[MAX_WORD_LENGTH] = "";
int currentLetterIndex = 0, currentLetterClock = 0;
bool gameActive = false;
unsigned long lastReadTime = 0;



// Функция запуска игры по внешнему событию
void startNewGame(int wordIndex) {
  if (wordIndex < 0 || wordIndex >= wordCount) return;

  strcpy(targetWord, words[wordIndex]);  // Выбираем слово
  currentLetterIndex = 0;                // Сбрасываем индекс
  gameActive = true;

  Serial.print("🎯 Загадано слово: ");
  Serial.println(targetWord);
}

// Функция отправки вибрации клиенту
void sendVibration(int strength, int duration) {
  struct {
    int vib;
    int timevib;
  } data = { strength, duration };

  radio.stopListening();
  radio.write(&data, sizeof(data));
  radio.startListening();
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  Wire.begin();
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(5, 15);
  radio.setAutoAck(true);

  radio.openWritingPipe(clientAddress);
  radio.openReadingPipe(1, serverAddress);
  radio.startListening();

  Serial.println("📡 Сервер RFID + nRF24L01 запущен...");
}

void loop() {
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

  if (radio.available()) {
    char receivedUID[15] = "";
    radio.read(&receivedUID, sizeof(receivedUID));



    char receivedLetter = searchInEEPROM(receivedUID);

    if (receivedLetter != '\0') {
    //  Serial.print("🔎 Найденная буква: ");
    //  Serial.println(receivedLetter);
    } else {
     // Serial.println("❌ UID не найден в базе!");
      return;
    }

    // Проверяем правильность буквы и задержку
    if (gameActive && millis() - lastReadTime > 600) {
      lastReadTime = millis();  // Обновляем таймер

      if (receivedLetter == targetWord[currentLetterIndex]) {
        Serial.println("✅ Верная буква!");
        currentLetterClock++ ;
         sendVibration(25, 390);  // Вибрация на 15 мс
        
        if (currentLetterClock == 15) {
          currentLetterClock = 0;
          currentLetterIndex++;
        }
        if (currentLetterIndex >= strlen(targetWord)) {
          Serial.println("🎉 Задание выполнено!");
          gameActive = false;
        }
      } else {
        currentLetterClock = 0;
        Serial.println("❌ Ошибка, ждём правильную букву!");
      }
    }
  }
}

char searchInEEPROM(const char* uid) {
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
    Wire.requestFrom(EEPROM_ADDR, 32);  // Запрашиваем 17 байт (UID + буква)

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

      return letterBuffer[0];  // Возвращаем букву, если UID совпал
    }
  }

  return "";  // Если не нашли совпадение
}
