/*
This code can be used to modify a block of data in a Mifare Ultralight or Ultralight C card.
*/

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9  // Пин rfid модуля RST
#define SS_PIN 10  // Пин rfid модуля SS

MFRC522 rfid(SS_PIN, RST_PIN);  // Объект rfid модуля
MFRC522::MIFARE_Key key;        // Объект ключа
MFRC522::StatusCode status;     // Объект статуса

byte id_to_write = 0, block_to_write = 0, sector_to_write = 0;

void setup() {
  Serial.begin(115200);                      // Инициализация Serial
  SPI.begin();                               // Инициализация SPI
  rfid.PCD_Init();                           // Инициализация модуля
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);  // Установка усиления антенны
  rfid.PCD_AntennaOff();                     // Перезагружаем антенну
  rfid.PCD_AntennaOn();                      // Включаем антенну
  for (byte i = 0; i < 6; i++) {             // Наполняем ключ
    key.keyByte[i] = 0xFF;                   // Ключ по умолчанию 0xFFFFFFFFFFFF
  }
}

void loop() {
  static uint32_t rebootTimer = millis();  // Важный костыль против зависания модуля!
  if (millis() - rebootTimer >= 1000) {    // Таймер с периодом 1000 мс
    rebootTimer = millis();                // Обновляем таймер
    digitalWrite(RST_PIN, HIGH);           // Сбрасываем модуль
    delayMicroseconds(2);                  // Ждем 2 мкс
    digitalWrite(RST_PIN, LOW);            // Отпускаем сброс
    rfid.PCD_Init();                       // Инициализируем заного
  }

  if (id_to_write == 0) {
    // Serial.println("Waiting for serial input");
    if (Serial.available() > 0) {                   // Check if data is available to read
      String input = Serial.readStringUntil('\n');  // Read the input until newline character

      // Split the input string into three parts
      int num_values = sscanf(input.c_str(), "%hhu %hhu %hhu", &block_to_write, &sector_to_write, &id_to_write);

      // Check if exactly three values were successfully parsed
      if (num_values != 3) {
        Serial.println("Error: Input format should be 'X X X' where each X is a byte value");
        return;
      }

      // Output the parsed values
      Serial.print("Block: ");
      Serial.println(block_to_write);
      Serial.print("Sector: ");
      Serial.println(sector_to_write);
      Serial.print("ID: ");
      Serial.println(id_to_write);
    }
  } else {
    // Serial.println("Waiting for RFID card");
    if (!rfid.PICC_IsNewCardPresent()) return;  // Если новая метка не поднесена - вернуться в начало loop
    if (!rfid.PICC_ReadCardSerial()) return;    // Если метка не читается - вернуться в начало loop

    /* Запись блока, указываем блок безопасности  */
    uint8_t secBlockDump[16] = { 0, 0, 0, 0,
                                 0, 0, 0, 0,
                                 0, 0, 0, 0,
                                 0, 0, 0, 0 };

    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    String card_type = rfid.PICC_GetTypeName(piccType);
    if (card_type == "MIFARE 1KB") {
      Serial.println("MIFARE 1KB card detected");

      // to write to certain cards, you need to authenticate sector
      // proper key has to be applied to the last block of data in sector
      // for sector 0, it will be block 3, for sector 1, it will be block 7, for sector 2 - block 11 etc.
      byte key_block = (block_to_write / 4) * 4 + 3;
      if (key_block == block_to_write) {
        Serial.println("Can't write to this block, it will ruin RFID card. Choose a different block.");
        return;
      }
      status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, key_block, &key, &(rfid.uid));
      if (status != MFRC522::STATUS_OK) {  // Если не окэй
        Serial.println("Auth error");      // Выводим ошибку
        return;
      }
    }

    secBlockDump[sector_to_write] = id_to_write;

    status = rfid.MIFARE_Write(block_to_write, secBlockDump, 16);  // Пишем массив в блок
    if (status != MFRC522::STATUS_OK) {                            // Если не окэй
      Serial.println("Write error");                               // Выводим ошибку
      return;
    } else id_to_write = 0;

    Serial.println("Write OK");  // Завершаем работу с меткой
    rfid.PICC_DumpToSerial(&(rfid.uid));
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}