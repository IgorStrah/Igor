#include <Wire.h>

#define EEPROM_SOURCE_ADDR  0x50  // Адрес первой EEPROM (с данными)
#define EEPROM_DEST_ADDR    0x52  // Адрес второй EEPROM (пустой)
#define EEPROM_SIZE         8192  // Размер AT24C64 (8 КБ)

void setup() {
  Wire.begin();
  Serial.begin(115200);
  
  Serial.println("Определение размера записанных данных...");
  int usedSize =EEPROM_SIZE;// getUsedMemorySize(EEPROM_SOURCE_ADDR);

  Serial.print("Используемый объем памяти: ");
  Serial.print(usedSize);
  Serial.println(" байт");

  if (usedSize > 0) {
    Serial.println("Копирование данных...");


    

  for (int address = 0; address < usedSize; address++) {
  //  uint8_t sourceData = readByteEEPROM(EEPROM_SOURCE_ADDR, address);
    delay(15);
    uint8_t destData = readByteEEPROM(0x51, address);
    delay(15);
   // if (sourceData != destData) {
    //  Serial.print("❌ Несовпадение по адресу 0x");
    //  Serial.print(address, HEX);
    //  Serial.print(": Исходные данные 0x");
      //Serial.print(sourceData, HEX);
      Serial.print(", Записанные данные 0x");
      Serial.println(destData, HEX);
   
  //  }
  }






    //copyEEPROM(EEPROM_SOURCE_ADDR, EEPROM_DEST_ADDR, usedSize);

    Serial.println("\nВерификация данных...");
    if (verifyEEPROM(EEPROM_SOURCE_ADDR, EEPROM_DEST_ADDR, usedSize)) {
      Serial.println("✅ Верификация пройдена, данные идентичны!");
    } else {
      Serial.println("❌ Ошибка верификации! Данные различаются.");
    }

    Serial.println("\nВывод записанных данных:");
    printEEPROM(EEPROM_DEST_ADDR, usedSize);
  } else {
    Serial.println("EEPROM пуста, копирование не требуется.");
  }
}

void loop() {}

// Определяет объем занятой памяти
int getUsedMemorySize(uint8_t deviceAddr) {
  for (int address = EEPROM_SIZE - 1; address >= 0; address--) {
    uint8_t data = readByteEEPROM(deviceAddr, address);
    if (data != 0xFF) {  
      return address + 1; 
    }
  }
  return 0; 
}

// Копирование данных EEPROM
void copyEEPROM(uint8_t sourceAddr, uint8_t destAddr, int dataSize) {
  const int pageSize = 32; // Размер страницы AT24C64
  uint8_t buffer[pageSize];

  for (int address = 0; address < dataSize; address += pageSize) {
    int chunkSize = min(pageSize, dataSize - address);
    
    readEEPROM(sourceAddr, address, buffer, chunkSize);
    writeEEPROM(destAddr, address, buffer, chunkSize);
     
    delay(5);
  }
}

// Верификация данных после записи
bool verifyEEPROM(uint8_t sourceAddr, uint8_t destAddr, int dataSize) {
  for (int address = 0; address < dataSize; address++) {
    uint8_t sourceData = readByteEEPROM(sourceAddr, address);
    uint8_t destData = readByteEEPROM(destAddr, address);

    if (sourceData != destData) {
      Serial.print("❌ Несовпадение по адресу 0x");
      Serial.print(address, HEX);
      Serial.print(": Исходные данные 0x");
      Serial.print(sourceData, HEX);
      Serial.print(", Записанные данные 0x");
      Serial.println(destData, HEX);
      return false;
    }
  }
  return true;
}

// Вывод всех записанных данных
void printEEPROM(uint8_t deviceAddr, int dataSize) {
  for (int address = 0; address < dataSize; address++) {
    uint8_t data = readByteEEPROM(deviceAddr, address);
    
    Serial.print(data, HEX);
    Serial.print(" ");
    
    if ((address + 1) % 16 == 0) Serial.println();
  }
  Serial.println();
}

// Чтение массива данных
void readEEPROM(uint8_t deviceAddr, int memAddr, uint8_t* buffer, int length) {
  Wire.beginTransmission(deviceAddr);
  Wire.write((memAddr >> 8) & 0xFF);
  Wire.write(memAddr & 0xFF);
  Wire.endTransmission();
  
  Wire.requestFrom(deviceAddr, length);
  for (int i = 0; i < length && Wire.available(); i++) {
    buffer[i] = Wire.read();
    
  }
}

// Запись массива данных
void writeEEPROM(uint8_t deviceAddr, int memAddr, uint8_t* buffer, int length) {
  for (int i = 0; i < length; i++) {
    Wire.beginTransmission(deviceAddr);
    Wire.write((memAddr >> 8) & 0xFF);
    Wire.write(memAddr & 0xFF);
    Wire.write(buffer[i]);
    Wire.endTransmission();
    
    memAddr++;
    delay(5);
  }
}

// Чтение одного байта
uint8_t readByteEEPROM(uint8_t deviceAddr, int memAddr) {
  Wire.beginTransmission(deviceAddr);
  Wire.write((memAddr >> 8) & 0xFF);
  Wire.write(memAddr & 0xFF);
  Wire.endTransmission();
  
  Wire.requestFrom(deviceAddr, 1);
  return Wire.available() ? Wire.read() : 0xFF;
}
