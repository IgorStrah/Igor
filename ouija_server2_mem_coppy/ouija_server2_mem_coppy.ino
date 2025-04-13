#include <Wire.h>

#define EEPROM_ADDR 0x50  // Адрес I2C для 24C65, обычно это 0x50 (для адреса 0xA0)

const int uidCount = 45;
const char rfidMap[uidCount][2][15] = {
    { "04BB8F1A237380", "!" }, //yes
    { "04C38F1A237380", "@" }, //ouija
    { "04BF8F1A237380", "#" }, //no
    { "042D8F1A237380", "a" }, 
    { "04318F1A237380", "b" }, 
    { "04358F1A237380", "c" }, 
    { "04398F1A237380", "d" }, 
    { "043D8F1A237380", "e" }, 
    { "04418F1A237380", "f" }, 
    { "04458F1A237380", "g" }, 
    { "04488F1A237380", "h" }, 
    { "044C8F1A237380", "i" }, 
    { "04508F1A237380", "j" }, 
    { "04548F1A237380", "k" }, 
    { "04588F1A237380", "l" }, 
    { "045C8F1A237380", "m" }, 
    { "04608F1A237380", "n" }, 
    { "04648F1A237380", "o" }, 
    { "04688F1A237380", "p" }, 
    { "046C8F1A237380", "q" }, 
    { "04708F1A237380", "r" }, 
    { "04748F1A237380", "s" }, 
    { "04788F1A237380", "t" }, 
    { "047C8F1A237380", "u" }, 
    { "04848F1A237380", "v" }, 
    { "04808F1A237380", "w" }, 
    { "04888F1A237380", "x" }, 
    { "0480C11A237381", "y" }, 
    { "04908F1A237380", "z" }, 
    { "04A08F1A237380", "1" }, 
    { "04A48F1A237380", "2" }, 
    { "04968E1A237380", "3" }, 
    { "049A8E1A237380", "4" }, 
    { "04A28E1A237380", "5" }, 
    { "04A68E1A237380", "6" }, 
    { "04B68F1A237380", "7" }, 
    { "049C8F1A237380", "8" }, 
    { "049C8F1A237380", "9" }, 
    { "04948F1A237380", "0" }, 
    { "04AC8F1A237380", "&" }, 
    { "04B18F1A237380", "&" }
};

void readFromEEPROM(int page) {
    int baseAddr = page * 32;  // Адрес начала страницы

    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write((baseAddr >> 8) & 0xFF);  // Старший байт адреса
    Wire.write(baseAddr & 0xFF);         // Младший байт адреса
    Wire.endTransmission();
    
    Wire.requestFrom(EEPROM_ADDR, 32);  // Читаем 32 байта (по 15 для UID и 2 для буквы)

    char uid[15];
    char letter[2];

    // Читаем UID
    for (int i = 0; i < 15; i++) {
        uid[i] = Wire.read();
    }

    // Читаем букву
    for (int i = 0; i < 2; i++) {
        letter[i] = Wire.read();
    }

    // Выводим считанные данные
    Serial.print("UID: ");
    Serial.print(uid);
    Serial.print(", Буква: ");
    Serial.println(letter);
}

void writeToEEPROM(int page, const char* uid, const char* letter) {
    int baseAddr = page * 32;  // 32 байта на одну страницу, 2 строки по 15 символов

    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write((baseAddr >> 8) & 0xFF);  // Старший байт адреса
    Wire.write(baseAddr & 0xFF);         // Младший байт адреса

    // Записываем UID
    for (int i = 0; i < 15; i++) {
        Wire.write(uid[i]);
    }

    // Записываем букву
    for (int i = 0; i < 2; i++) {
        Wire.write(letter[i]);
    }

    Wire.endTransmission();
    delay(5);  // Задержка на запись в память
}

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Записываем данные в память
    for (int i = 0; i < uidCount; i++) {
        writeToEEPROM(i, rfidMap[i][0], rfidMap[i][1]);
        Serial.print("Записано: UID = ");
        Serial.print(rfidMap[i][0]);
        Serial.print(", Буква = ");
        Serial.println(rfidMap[i][1]);
        delay(10);
    }

    Serial.println("Данные записаны в EEPROM.");

     for (int i = 0; i < 45; i++) {
        readFromEEPROM(i);  // Читаем и выводим данные для каждой страницы
        delay(10);  // Задержка для удобства
    }
    delay(50000);
}

void loop() {
    // В этом примере записываем данные при старте, в loop() ничего не делаем.
}