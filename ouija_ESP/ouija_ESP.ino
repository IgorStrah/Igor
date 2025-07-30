#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <gpio.h>
#include "WebControl.h"
#define DEBUG
const char* ssid = "LMT-5DA0";
const char* password = "IH7R9QCREC";
char uidBuffer[16] = "NoCard";  // Буфер для UID (если нет карты)
char activeUID[16] = "";
int currentPhraseIndex = -1;
int currentLetterIndex = 0;
int currentLetterClock = 0;
String expectedLetterUID = "";
String currentPhrase = "";
String triggerUID = "";
String nextExpectedLetterUID = "";


String debugLog = "";  // Буфер для логов
bool wifiConnected;
bool isAccessPoint = false;
#define RST_PIN 1           // Configurable, see typical pin layout above
#define SS_PIN 3            // Configurable, see typical pin layout above
#define IRQ_PIN 5           // GPIO5 (D1) - прерывание от RC522
#define pwmPin 4            // GPIO4
const int pwmFreq = 1000;   // Частота PWM в Гц
const int pwmRange = 1023;  // Разрешение ШИМ (максимум)
unsigned long vibroEndTime = 0;


ESP8266WebServer server(80);
String phrases[10];  // теперь редактируемые


MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

unsigned long lastActivityTime = 0;
const unsigned long sleepDelay = 4120000;  // 1 минута (60 000 мс)


void setup() {
  pinMode(pwmPin, OUTPUT);
  analogWriteRange(pwmRange);  // Установка диапазона (по умолчанию 1023)
  analogWriteFreq(pwmFreq);    // Установка частоты PWM
  analogWrite(pwmPin, 0);      // Выключить ШИМ на старте



  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 5000;

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    delay(100);
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
  } else {
    WiFi.softAP("ESP-ouija", "12345678");
    isAccessPoint = true;
  }

  setupWebServer(server, wifiConnected, phrases, 10, debugLog);

  if (wifiConnected) {
    ArduinoOTA.setHostname("esp12-ota");
    ArduinoOTA.setPassword("262626");
    ArduinoOTA.begin();
  }




  SPI.begin();         // Инициализация SPI
  mfrc522.PCD_Init();  // Инициализация RC522
  delay(4);
  lastActivityTime = millis();
}
void loop() {
  handleWebLoop();
  if (wifiConnected) ArduinoOTA.handle();

  if (millis() >= vibroEndTime) {
    analogWrite(pwmPin, 0);
  }

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Чтение UID
    memset(uidBuffer, 0, sizeof(uidBuffer));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      sprintf(uidBuffer + i * 2, "%02X", mfrc522.uid.uidByte[i]);
    }

    String scannedUID = String(uidBuffer);
    logDebug("🧬 UID Now: " + scannedUID);
    mfrc522.PCD_StopCrypto1();
    lastActivityTime = millis();

    // Специальный UID для сна
    if (scannedUID == "71083BFF") {
      logDebug("💤 Sleep UID detected.");
      delay(100);
      mfrc522.PCD_SoftPowerDown();
      delay(100);
      ESP.deepSleep(0);
    }

 if (currentPhrase.isEmpty()) {
  String loadedPhrase = loadPhraseByTriggerUID(scannedUID);
  if (loadedPhrase.length() > 0) {
    currentPhrase = loadedPhrase;
    triggerUID = scannedUID;
    currentLetterIndex = 0;
    currentLetterClock = 0;
    expectedLetterUID = getUIDBySymbolFromFile(currentPhrase[currentLetterIndex]);
    nextExpectedLetterUID = (currentLetterIndex + 1 < currentPhrase.length())
      ? getUIDBySymbolFromFile(currentPhrase[currentLetterIndex + 1])
      : "";

    vibrate(10 + currentLetterIndex, 100);
    logDebug("▶️ New phrase: " + currentPhrase);
    logDebug("🔡 Expect: " + String(currentPhrase[currentLetterIndex]));
    logDebug("🧬 UID: " + expectedLetterUID);
    return;
  }
} else if (scannedUID == triggerUID) {
  // Повторный скан начального UID — сброс фразы
  currentLetterIndex = 0;
  currentLetterClock = 0;
  expectedLetterUID = getUIDBySymbolFromFile(currentPhrase[0]);
  nextExpectedLetterUID = (currentPhrase.length() > 1)
    ? getUIDBySymbolFromFile(currentPhrase[1])
    : "";

  vibrate(20, 200);  // Индикация сброса
  logDebug("🔁 Phrase restarted by repeated trigger UID.");
  return;
}

    // Обработка текущей фразы
    if (!currentPhrase.isEmpty()) {
      bool isCurrent = (scannedUID == expectedLetterUID);
      bool isNext = (scannedUID == nextExpectedLetterUID);

      if (isCurrent) {
        currentLetterClock++;
        vibrate(10 + currentLetterClock, 200);
        delay(10);

        if (currentLetterClock >= 15) {
          currentLetterIndex++;
          currentLetterClock = 0;
        }
      }

      if (currentLetterClock >= 5 && isNext) {
        currentLetterIndex++;
        currentLetterClock = 1;
        vibrate(30, 200);
        delay(10);
      }

      // Проверка завершения фразы
      if (currentLetterIndex >= currentPhrase.length()) {
        logDebug("✅ Phrase complete.");
        currentPhrase = "";
        expectedLetterUID = "";
        nextExpectedLetterUID = "";
      } else {
        expectedLetterUID = getUIDBySymbolFromFile(currentPhrase[currentLetterIndex]);
        nextExpectedLetterUID = (currentLetterIndex + 1 < currentPhrase.length())
                                  ? getUIDBySymbolFromFile(currentPhrase[currentLetterIndex + 1])
                                  : "";

        logDebug("🔡 Now expecting: " + String(currentPhrase[currentLetterIndex]));
        logDebug("🧬 UID: " + expectedLetterUID);
      }

      return;
    }
  }

  // Таймер простоя
  if (millis() - lastActivityTime > sleepDelay) {
    logDebug("🕒 Inactivity timeout. Sleeping.");
    mfrc522.PCD_SoftPowerDown();
    delay(100);
    ESP.deepSleep(0);
  }
}
String loadPhraseByTriggerUID(String uid) {
  File file = LittleFS.open("/phrases.txt", "r");
  if (!file) return "";

  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;

    int comma = line.indexOf(',');
    if (comma == -1) continue;

    String lineUID = line.substring(0, comma);
    String phrase = line.substring(comma + 1);

    if (lineUID == uid) {
      file.close();
      return phrase;
    }
  }

  file.close();
  return "";  // UID не найден
}


String getUIDBySymbolFromFile(char symbol) {
  File file = LittleFS.open("/rfidmap.txt", "r");
  if (!file) {
    return "";
  }
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();  // удалим \r и пробелы

    if (line.length() == 0) continue;

    int commaIndex = line.indexOf(',');
    if (commaIndex == -1) continue;

    String uid = line.substring(0, commaIndex);
    String sym = line.substring(commaIndex + 1);

    if (sym.length() == 1 && sym[0] == symbol) {
      file.close();
      return uid;
    }
  }
  file.close();
  return "";  // Не найдено
}


void logDebug(const String& msg) {
#ifdef DEBUG
  debugLog += msg + "\n";
  const size_t maxSize = 2000;
  if (debugLog.length() > maxSize) {
    debugLog = debugLog.substring(debugLog.length() - maxSize / 2);
  }
#endif
}


void vibrate(uint8_t strength, unsigned long durationMs) {
  analogWrite(pwmPin, strength);
  vibroEndTime = millis() + durationMs;
}