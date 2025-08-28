#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

//#define DEBUG
#define CH_NUM 0x95
#define SIG_POWER RF24_PA_HIGH
#define SIG_SPEED RF24_1MBPS
#define EEPROM_ADDR 0x50

RF24 radio(9, 10);

const char* expectedUIDs[] = {
  "04248E1A237380", 
  "B684063E",
  "FF0FB0FD020000", 
  "04288E1A237380",
  "04B5CB6DBC2A81",
  "FF0F50D5020000",
  "410C3BFF",
  "C3D4063E",
  "D4E5063E",
  "E5F6063E"
};
 
const char* phrases[] = {
  "@latvijaspasakas13%",
  "@darknesslumos%",
  "@heat60%",
  "@lighteye%",
  "@greenwhitebluewhite%",  
  "@picturewall%",
  "@completesilence",
  "@completesilence",
  "@BEGIN PUZZLE",
  "@ESCAPE ROOM"
};

const int uidCount = sizeof(expectedUIDs) / sizeof(expectedUIDs[0]);

byte pipeNo;
byte address[][6] = { "1Node" };
int telemetry[2];

char activeUID[16] = "";
int currentPhraseIndex = -1;
int currentLetterIndex = 0;
int currentLetterClock = 0;
String expectedLetterUID = "";

void setup() {
  Wire.begin();
  Serial.begin(115200);
  radioSetup();
  Serial.println("System ready.");
}

void loop() {
  if (radio.available(&pipeNo)) {
    char receivedUID[16] = "";
    telemetry[0] = 0;
    telemetry[1] = 0;
    radio.read(&receivedUID, sizeof(receivedUID));
    radio.flush_rx();

#ifdef DEBUG
    Serial.print("📥 Received UID: ");
    Serial.println(receivedUID);
#endif

    int index = findUIDIndex(receivedUID);

    if (index != -1) {
      // Сканирован один из 10 стартовых UID
      if (strcmp(activeUID, receivedUID) != 0) {
        // Новый UID — начинаем новую фразу
        strcpy(activeUID, receivedUID);
        currentPhraseIndex = index;
        currentLetterIndex = 0;
        expectedLetterUID = searchInEEPROM(phrases[currentPhraseIndex][currentLetterIndex]);
              telemetry[0] = 10 + currentLetterIndex;
      telemetry[1] = 1000;
#ifdef DEBUG
        Serial.print("➡️ New phrase: ");
        Serial.println(phrases[currentPhraseIndex]);
        Serial.print("🔡 Looking for letter: ");
        Serial.println(phrases[currentPhraseIndex][currentLetterIndex]);
        Serial.print("🧬 UID for letter: ");
        Serial.println(expectedLetterUID);
#endif
      } else {
        // Повторный скан того же UID — сброс
        currentLetterIndex = 0;
        expectedLetterUID = searchInEEPROM(phrases[currentPhraseIndex][currentLetterIndex]);
      telemetry[0] = 25 ;
      telemetry[1] = 333;
#ifdef DEBUG
        Serial.println("🔄 Rescanned same UID. Progress reset.");
#endif
      }
    } else if (currentPhraseIndex != -1 && expectedLetterUID.equals(receivedUID)) {

       if (currentLetterClock == 10) {
        currentLetterClock = 0;
        currentLetterIndex++;
 
      }

 
#ifdef DEBUG
      Serial.println("✅ Correct letter UID!");
#endif
     currentLetterClock++;
      telemetry[0] = 15 + currentLetterClock * 2;
      telemetry[1] = 1400;
      if (phrases[currentPhraseIndex][currentLetterIndex] == '\0') {
#ifdef DEBUG
        Serial.println("🎉 Phrase complete!");
#endif
        currentPhraseIndex = -1;
        activeUID[0] = '\0';
      } else {
        expectedLetterUID = searchInEEPROM(phrases[currentPhraseIndex][currentLetterIndex]);
#ifdef DEBUG
        Serial.print("🔡 Next letter: ");
        Serial.println(phrases[currentPhraseIndex][currentLetterIndex]);
        Serial.print("🧬 UID for letter: ");
        Serial.println(expectedLetterUID);
#endif
      }
    } else {
      currentLetterClock=0;
      telemetry[0] = 1;
      telemetry[1] = 1;
#ifdef DEBUG
      Serial.println("❌ Incorrect UID.");
#endif
    }

    // Отправка телеметрии обратно
    radio.writeAckPayload(pipeNo, &telemetry, sizeof(telemetry));
  }
}

// Найти индекс UID в массиве expectedUIDs[]
int findUIDIndex(const char* uid) {
  for (int i = 0; i < uidCount; i++) {
    if (strcmp(uid, expectedUIDs[i]) == 0) return i;
  }
  return -1;
}

// Поиск UID по букве в EEPROM
String searchInEEPROM(char letter) {
  char uidBuffer[15];
  char letterBuffer[2];
  int pageCount = 45;

  for (int page = 0; page < pageCount; page++) {
    int baseAddr = page * 32;
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write((baseAddr >> 8) & 0xFF);
    Wire.write(baseAddr & 0xFF);
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_ADDR, 32);

    for (int i = 0; i < 15; i++) uidBuffer[i] = Wire.read();
    for (int i = 0; i < 2; i++) letterBuffer[i] = Wire.read();

    if (letterBuffer[0] == letter) {
      uidBuffer[14] = '\0';
      return String(uidBuffer);
    }
  }
  return "";
}

// Настройка радио
void radioSetup() {
  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(5, 15);
  radio.enableAckPayload();
  radio.setPayloadSize(32);
  radio.openReadingPipe(1, address[0]);
  radio.setChannel(CH_NUM);
  radio.setPALevel(SIG_POWER);
  radio.setDataRate(SIG_SPEED);
  radio.powerUp();
  radio.startListening();
}
