#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <gpio.h>
#define DEBUG
const char* ssid = "LMT-5DA0";
const char* password = "IH7R9QCREC";
char uidBuffer[16] = "NoCard";  // Буфер для UID (если нет карты)
char activeUID[16] = "";
int currentPhraseIndex = -1;
int currentLetterIndex = 0;
int currentLetterClock = 0;
String expectedLetterUID = "";
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
const char* expectedUIDs[] = {
  "04248E1A237380",
  "B684063E",
  "FF0FB0FD020000",
  "04288E1A237380",
  "04B5CB6DBC2A81",
  "FF0F50D5020000",
  "B2C3063E",
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
  "@SAFE UNLOCKED",
  "@ENTER PORTAL",
  "@BEGIN PUZZLE",
  "@ESCAPE ROOM"
};
const int uidCount = sizeof(expectedUIDs) / sizeof(expectedUIDs[0]);
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

unsigned long lastActivityTime = 0;
const unsigned long sleepDelay = 4120000;  // 1 минута (60 000 мс)

ESP8266WebServer server(80);
File fsUploadFile;
// HTML-страница для управления файлами
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head><meta charset="UTF-8"><title>File Manager</title></head>
<body>
  <h2>📁 ESP8266 File Manager</h2>
    <p>
    <a href="/debug" target="_blank">📋 Перейти к отладке</a>
 
  </p>
  <form method="POST" action="/upload" enctype="multipart/form-data">
    <input type="file" name="upload">
    <input type="submit" value="Upload">
  </form>
  <hr>
  <div id="filelist"></div>
  <script>
    async function listFiles() {
      const res = await fetch("/list");
      const files = await res.json();
      let html = "<ul>";
      files.forEach(file => {
        html += `<li>
          <a href="${file.name}" target="_blank">${file.name}</a> 
          [<a href="#" onclick="editFile('${file.name}')">edit</a>] 
          [<a href="/delete?name=${file.name}">delete</a>]
        </li>`;
      });
      html += "</ul>";
      document.getElementById("filelist").innerHTML = html;
    }

    function editFile(name) {
      fetch(name).then(r => r.text()).then(text => {
        const newText = prompt("Edit file content:", text);
        if (newText !== null) {
          fetch("/edit", {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: "name=" + encodeURIComponent(name) + "&content=" + encodeURIComponent(newText)
          }).then(() => listFiles());
        }
      });
    }

    listFiles();
  </script>
</body>
</html>
)rawliteral";

// Возвращает список файлов в JSON
void handleFileList() {
  String output = "[";
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    if (output != "[") output += ',';
    output += "{\"name\":\"" + dir.fileName() + "\",\"size\":" + String(dir.fileSize()) + "}";
  }
  output += "]";
  server.send(200, "application/json", output);
}

// Загрузка файла
void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = "/" + upload.filename;
    fsUploadFile = LittleFS.open(filename, "w");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile) fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) fsUploadFile.close();
  }
}

// Удаление файла
void handleDelete() {
  if (server.hasArg("name")) {
    LittleFS.remove(server.arg("name"));
    server.send(200, "text/plain", "Deleted");
  } else {
    server.send(400, "text/plain", "Missing file name");
  }
}

// Редактирование файла
void handleEdit() {
  if (server.hasArg("name") && server.hasArg("content")) {
    File f = LittleFS.open(server.arg("name"), "w");
    f.print(server.arg("content"));
    f.close();
    server.send(200, "text/plain", "Edited");
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

void setup() {
  pinMode(pwmPin, OUTPUT);
  analogWriteRange(pwmRange);  // Установка диапазона (по умолчанию 1023)
  analogWriteFreq(pwmFreq);    // Установка частоты PWM
  analogWrite(pwmPin, 0);      // Установка скважности 50%

  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522
  delay(4);
  lastActivityTime = millis();



WiFi.begin(ssid, password);
unsigned long startAttemptTime = millis();
const unsigned long wifiTimeout = 5000;  // 5 секунд

while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
  delay(100);
}

if (WiFi.status() == WL_CONNECTED) {
  wifiConnected = true;
} else {
  // Не удалось подключиться — поднимаем точку доступа
  WiFi.softAP("ESP-ouija", "12345678");  // SSID, пароль
  isAccessPoint = true;
}


if (wifiConnected) {
  // Только если подключились — инициализируем OTA и сервер
  if (LittleFS.begin()) {
    server.on("/", HTTP_GET, []() {
      server.send_P(200, "text/html", index_html);
    });

    server.on("/list", HTTP_GET, handleFileList);
    server.on("/upload", HTTP_POST, []() {
      server.send(200, "text/plain", "Uploaded");
    }, handleFileUpload);
    server.on("/delete", HTTP_GET, handleDelete);
    server.on("/edit", HTTP_POST, handleEdit);
    server.on("/debug", HTTP_GET, []() {
      String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Debug Log</title></head><body>";
      html += "<h2>📋 Отладочный лог</h2><pre>" + debugLog + "</pre>";
      html += "<meta http-equiv='refresh' content='2'>";
      html += "</body></html>";
      server.send(200, "text/html", html);
    });
    server.on("/clearlog", HTTP_GET, []() {
      debugLog = "";
      server.send(200, "text/plain", "Log cleared");
    });
    server.onNotFound([]() {
      String path = server.uri();
      if (LittleFS.exists(path)) {
        File file = LittleFS.open(path, "r");
        server.streamFile(file, "text/plain");
        file.close();
      } else {
        server.send(404, "text/plain", "File Not Found");
      }
    });

    server.begin();
  }

  ArduinoOTA.setHostname("esp12-ota");
  ArduinoOTA.setPassword("262626");
  ArduinoOTA.begin();
}
}

void loop() {

  if (wifiConnected) {
  server.handleClient();
  ArduinoOTA.handle();
}


  if (millis() >= vibroEndTime) {
    analogWrite(pwmPin, 0);  // выключить вибрацию
  }
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Читаем UID карты
    memset(uidBuffer, 0, sizeof(uidBuffer));
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      sprintf(uidBuffer + i * 2, "%02X", mfrc522.uid.uidByte[i]);
    }
    // strcpy(activeUID, "410C3BFF");
    if (strcmp("410C3BFF", uidBuffer) == 0) {
      analogWrite(pwmPin, 55);
      delay(1000);
      analogWrite(pwmPin, 0);
      delay(1000);
    }
      logDebug("🧬 UID Now: " + String(uidBuffer));

    mfrc522.PCD_StopCrypto1();
    lastActivityTime = millis();  // сброс таймера
    int index = findUIDIndex(uidBuffer);
    if (index != -1) {
      // Сканирован один из 10 стартовых UID
      if (strcmp(activeUID, uidBuffer) != 0) {
        // Новый UID — начинаем новую фразу
        strcpy(activeUID, uidBuffer);
        currentPhraseIndex = index;
        currentLetterIndex = 0;
        expectedLetterUID = getUIDBySymbolFromFile(phrases[currentPhraseIndex][currentLetterIndex]);

        vibrate(10 + currentLetterIndex, 100);

#ifdef DEBUG
        logDebug("➡️ New phrase: ");
        logDebug(phrases[currentPhraseIndex]);
        logDebug("🔡 Looking for letter: ");
        logDebug(String(phrases[currentPhraseIndex][currentLetterIndex]));
        logDebug("🧬 UID for letter: ");
        logDebug(expectedLetterUID);


#endif

      } else {
        // Повторный скан того же UID — сброс
        currentLetterIndex = 0;
        expectedLetterUID = getUIDBySymbolFromFile(phrases[currentPhraseIndex][currentLetterIndex]);
      }
    } else if (currentPhraseIndex != -1 && expectedLetterUID==uidBuffer) {
      logDebug("✅ Correct letter UID! ");
      logDebug(uidBuffer);
      vibrate(15 + currentLetterClock * 2, 100);
      if (currentLetterClock == 15) {
        currentLetterClock = 0;
        currentLetterIndex++;
      }
      delay(50);
      currentLetterClock++;
      if (phrases[currentPhraseIndex][currentLetterIndex] == '\0') {

        currentPhraseIndex = -1;
        activeUID[0] = '\0';
      } else {
        expectedLetterUID = getUIDBySymbolFromFile(phrases[currentPhraseIndex][currentLetterIndex]);
#ifdef DEBUG
        logDebug("🔡 Next letter: ");
        logDebug(String(phrases[currentPhraseIndex][currentLetterIndex]));
        logDebug("🧬 UID for letter: ");
        logDebug(expectedLetterUID);
#endif

      }
    } else {
      currentLetterClock = 0;
    }
  }

  strcpy(activeUID, "71083BFF");
  if (strcmp(activeUID, uidBuffer) == 0) {
    logDebug("  SLEEP: ");
    delay(100);
    mfrc522.PCD_SoftPowerDown();
    delay(100);
    ESP.deepSleep(0);
  }




  // Условие простоя более
  if (millis() - lastActivityTime > sleepDelay) {
    mfrc522.PCD_SoftPowerDown();
    delay(100);
    ESP.deepSleep(0);
  }
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

// Найти индекс UID в массиве expectedUIDs[]
int findUIDIndex(const char* uid) {
  for (int i = 0; i < uidCount; i++) {
    if (strcmp(uid, expectedUIDs[i]) == 0) return i;
  }
  return -1;
}

void logDebug(String msg) {
  debugLog += msg + "\n";
  // Ограничим размер лога, чтобы не переполнить память
  const size_t maxSize = 2000;
  if (debugLog.length() > maxSize) {
    debugLog = debugLog.substring(debugLog.length() - maxSize / 2);
  }
}


void vibrate(uint8_t strength, unsigned long durationMs) {
  analogWrite(pwmPin, strength);
  vibroEndTime = millis() + durationMs;
}