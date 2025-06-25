//#define DEBUG
#include <DFPlayerMini_Fast.h>
#include <CD74HC4067.h>
CD74HC4067 my_mux(2, 3, 4, 5);
const int g_common_pin = 6;
const int light = 10;
#include <SoftwareSerial.h>
SoftwareSerial mySerial(A2, A3);  // RX, TX
DFPlayerMini_Fast myMP3;

//--------------------- НАСТРОЙКИ ----------------------
#define CH_NUM 0x95  // номер канала (должен совпадать с передатчиком)
//--------------------- НАСТРОЙКИ ----------------------
// На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
#define SIG_POWER RF24_PA_HIGH
// СКОРОСТЬ ОБМЕНА
#define SIG_SPEED RF24_1MBPS
//--------------------- ДЛЯ РАЗРАБОТЧИКОВ -----------------------

//--------------------- БИБЛИОТЕКИ ----------------------
#include <iarduino_IR_TX.h>
#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#define EEPROM_ADDR 0x50  // Адрес I2C для 24C65, обычно это 0x50 (для адреса  0xA0)

RF24 radio(9, 8);  // "создать" модуль на пинах 9 и 10 для НАНО/УНО
//--------------------- БИБЛИОТЕКИ ----------------------

//--------------------- ПЕРЕМЕННЫЕ ----------------------
byte pipeNo;
byte address[][6] = { "1Node", "2Node", "3Node", "4Node", "5Node", "6Node" };  // возможные номера труб
String serialInput = "";
struct Step {
  const char* word;
  void (*procedure)();
  const char* expectedUID;
};

// Переменные для логики игры

int currentLetterIndex = 0, currentLetterClock = 0;
bool gameActive = false;
unsigned long lastReadTime = 0;
unsigned long lightflash = 0;
int lightflashclock;
byte tempclocll;
bool CnockDor;
int telemetry[2];                            // массив данных телеметрии (то что шлём на передатчик)
const int micPin = A0;                       // Пин, к которому подключен микрофон
const int knockThreshold = 350;              // Порог срабатывания микрофона
const int requiredKnocks = 6;                // Количество стуков
const unsigned long knockTimeout = 2500;     // Максимальная пауза между стуками (мс)
const unsigned long minKnockInterval = 200;  // Минимальное время между стуками (мс)
int knockCount = 0;
unsigned long lastKnockTime = 0;
unsigned long lastReminderTime = 0;
const unsigned long reminderInterval = 60000;  // 60 секунд

int x;

const char* expectedUIDs[] = {
  "4DCD063E",
  "0289063E",
  "91CD063E",
  "53CD063E",
  "DCA8063E"
};
const int totalSteps = sizeof(expectedUIDs) / sizeof(expectedUIDs[0]);

int currentStepFin = 0;
char lastUID[16] = "123123";  // Для фильтрации повторов

void procedure1() {

  digitalWrite(g_common_pin, LOW);
  my_mux.channel(0);
  delay(1000);
  digitalWrite(g_common_pin, LOW);
}

void procedure2() {
  CnockDor = 1;
}
void procedure3() {
  digitalWrite(g_common_pin, LOW);
  my_mux.channel(2);
  delay(10000);
}

void procedure4() {
  digitalWrite(g_common_pin, HIGH);
  my_mux.channel(3);
  delay(1000);
  digitalWrite(g_common_pin, HIGH);
  my_mux.channel(15);
  delay(1000);
}

void procedure5() {
  delay(1000);
}
void procedure6() {
  delay(1000);
  myMP3.play(10);
}

const int stepSequence[] = { 1, 3, 5, 2, 4, 1 };  // порядок шагов
Step steps[] = {
  { "@clock1815%", procedure1, "19AD063E" },
  { "@knockdoor6%", procedure2, "3FA8063E" },
  { "@exploreshelf%", procedure3, "4DCD063E" },
  { "@insideshelf%", procedure3, "0289063E" },
  { "@usekey%", procedure4, "91CD063E" },
  { "@secretknock%", procedure5, "53CD063E" },
  { "@dolleg%", procedure6, "DCA8063E" },
  { "@blackmugheat%", procedure5, "4DCD063E" }
};

// Step steps[] = {
//   { "@clock1815%", procedure1, "19AD063E" },

// };
const int stepCount = sizeof(steps) / sizeof(steps[0]);

int currentStep = 0;
bool waitingForWord = true;
bool waitingForWordwite = false;
bool waitingForUID = false;
bool waitingPentacle = false;
String expectedUID = "";

void setup() {
  mySerial.begin(9600);
  myMP3.begin(mySerial, true);
  myMP3.volume(28);
  pinMode(light, OUTPUT);
  analogWrite(light, 0);
  pinMode(g_common_pin, OUTPUT);
  pinMode(micPin, INPUT);

  Wire.begin();
  Serial.begin(115200);
  radioSetup();
  Serial.print(F("Searching for word: "));
  Serial.println(steps[currentStep].word);
  expectedUID = searchInEEPROM(steps[currentStep].word[currentLetterIndex]);
  digitalWrite(g_common_pin, HIGH);
  my_mux.channel(0);
  CnockDor = 0;
}

void loop() {

  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      // Конец строки — обрабатываем команду
      handleSerialCommand(serialInput);
      serialInput = ""; // Очистка после обработки
    } else {
      serialInput += c;
    }
  }


  if (CnockDor == 1) {
    if (analogRead(micPin) > knockThreshold) {
      // Проверяем: достаточно ли времени прошло после последнего стука
      if (millis() - lastKnockTime >= minKnockInterval) {
        if (knockCount == 0 || (millis() - lastKnockTime <= knockTimeout)) {
          knockCount++;
          lastKnockTime = millis();
        } else {
          knockCount = 1;
          lastKnockTime = millis();
        }
      }
    }

    // Проверка на достижение нужного количества стуков
    if (knockCount >= requiredKnocks) {
      for (int i = 0; i < 2; i++) {
        my_mux.channel(1);
        delay(2000);
        my_mux.channel(16);
        delay(2000);
      }
      
//NEW!
           currentStep++;
      currentLetterIndex = 0;
      waitingForUID = false;
      waitingForWord = true;
      telemetry[0] = 40;
      telemetry[1] = 1000;

      
      myMP3.play(11);
      CnockDor = 0;
      knockCount = 0;
    }

    // Сброс по таймауту
    if (knockCount > 0 && millis() - lastKnockTime > knockTimeout) {
      knockCount = 0;
    }
  }




  if (waitingForWord) {
    Serial.print("Next letter: ");
    Serial.println(steps[currentStep].word[currentLetterIndex]);
    expectedUID = searchInEEPROM(steps[currentStep].word[currentLetterIndex]);
    waitingForWord = false;
  }

  if (millis() - lightflash > 60) {
    lightflash = millis();
    if (lightflashclock == 8990) { myMP3.play(8); }

    if (lightflashclock == 20) { myMP3.play(9); }
    if (lightflashclock > 1) {
      lightflashclock--;
      analogWrite(light, random(215, 255));
    } else {
      analogWrite(light, 0);
    }
  }


  if (radio.available(&pipeNo)) {  // слушаем эфир
    char receivedUID[16] = "";
    telemetry[0] = 0;
    telemetry[1] = 0;
    radio.read(&receivedUID, sizeof(receivedUID));

 
    for (int i = 0; i < 5; i++) {
      if (strcmp(receivedUID, expectedUIDs[i]) == 0 && waitingPentacle == false) {
        Serial.println(expectedUIDs[i]);
        myMP3.play(i + 2);
        delay(2000);  // Пауза, чтобы не проигрывать мелодию повторно сразу
        break;
      }
    }

#ifdef DEBUG
    Serial.print("📤 UID  серверу: ");
    Serial.println(receivedUID);
    Serial.print("🔎 wait  UID: ");
    Serial.println(expectedUID);
#endif

    if (strcmp(receivedUID, "3FA8063E") == 0) {
      lightflashclock = 9000;
    }
    if (strcmp(receivedUID, "19AD063E") == 0) {
      currentLetterIndex = 0;
      waitingForWord = true;
      waitingForWordwite = false;
    }

    if (expectedUID == receivedUID) {

#ifdef DEBUG
      Serial.println("✅ Correct UID detected!");
#endif
      currentLetterClock++;
      telemetry[0] = 15 + currentLetterClock * 2;
      telemetry[1] = 1400;

if (strcmp(receivedUID, "04AC8F1A237380") == 0) {
      currentLetterClock = 10;
    }

      if (currentLetterClock == 10) {
        currentLetterClock = 0;
        currentLetterIndex++;
        waitingForWordwite = true;
      }

      if (steps[currentStep].word[currentLetterIndex] == '\0' && waitingForUID == false) {

        telemetry[0] = 55;
        telemetry[1] = 2100;
        steps[currentStep].procedure();  // Выполняем процедуру
        waitingForUID = true;            // Ждём новый UID
        waitingForWord = false;

      } else {
        if (waitingForWordwite == true) {
          waitingForWord = true;  // Переход к следующей букве
          waitingForWordwite = false;
        }
      }

    } else if (waitingForUID && strcmp(receivedUID, steps[currentStep].expectedUID) == 0) {
#ifdef DEBUG
      Serial.print("✅ Expected UID received  ");
      Serial.println(steps[currentStep].expectedUID);
#endif
      myMP3.play(10);
      currentStep++;
      currentLetterIndex = 0;
      waitingForUID = false;
      waitingForWord = true;
      telemetry[0] = 40;
      telemetry[1] = 1000;
      if (currentStep >= stepCount) {
        waitingPentacle = true;
        waitingForUID = false;
        waitingForWord = false;
      }
    } else {

      currentLetterClock = 0;
      telemetry[0] = 1;
      telemetry[1] = 1;

#ifdef DEBUG
      Serial.println("❌ Incorrect UID, waiting for the correct one.");
#endif
    }


    if (waitingPentacle == true) {
      // Игнорировать повтор предыдущего UID
      if (strcmp(receivedUID, lastUID) == 0) {
        Serial.println(" lastUID = receivedUID!");
      }
      else {
        strcpy(lastUID, receivedUID);  // обновить последний UID
        telemetry[0] = 55;
        telemetry[1] = 1100;


        // Проверка на правильный UID
        if (strcmp(receivedUID, expectedUIDs[stepSequence[currentStepFin] - 1]) == 0) {
          Serial.print("Step ");
          Serial.print(currentStepFin + 1);

          currentStepFin++;

          if (currentStepFin >= totalSteps) {

            myMP3.loop(7);
            delay(100);
            myMP3.loop(7);
            currentStepFin = 0;  // Сброс для следующей попытки
          }
        } else {

          currentStepFin = 0;  // Сброс прогресса
        }
      }
    }



    // отправляем пакет телеметрии
    radio.writeAckPayload(pipeNo, &telemetry, sizeof(telemetry));
    radio.flush_rx();
  }
}

// Функция для поиска UID в памяти 24C64 и возврата соответствующей буквы
String searchInEEPROM(char letter) {
  char uidBuffer[15];
  char letterBuffer[2];
  int pageCount = 45;  // Количество страниц для поиска (каждая страница по 32 байта)
#ifdef DEBUG
  Serial.print("(letter  ");
  Serial.println(letter);
#endif
  for (int page = 0; page < pageCount; page++) {
    int baseAddr = page * 32;  // Каждый элемент UID + буква занимает 17 байт
    // Читаем данные с EEPROM
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write((baseAddr >> 8) & 0xFF);  // Старший байт адреса
    Wire.write(baseAddr & 0xFF);         // Младший байт адреса
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_ADDR, 32);  // Запрашиваем 17 байт (UID + буква)

    for (int i = 0; i < 15; i++) {
      uidBuffer[i] = Wire.read();
    }

    // Читаем букву
    for (int i = 0; i < 2; i++) {
      letterBuffer[i] = Wire.read();
    }

    if (letterBuffer[0] == letter) {
#ifdef DEBUG
      Serial.print("(letterBuffer[0]  ");
      Serial.println(letterBuffer[0]);
      Serial.print("uidBuffer  ");
      Serial.println(uidBuffer);
#endif
      if (uidBuffer == "") {
        Serial.print("(letterBuffer[0]  ");
        Serial.println(letterBuffer[0]);
        Serial.print("ERROR  ");
        delay(10000);
      }
      return String(uidBuffer);
    }
  }

  return "";  // Если не нашли совпадение
}


void radioSetup() {                      // настройка радио
  radio.begin();                         // активировать модуль
  radio.setAutoAck(1);                   // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(5, 15);               // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();              // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);              // размер пакета, байт
  radio.openReadingPipe(1, address[0]);  // хотим слушать трубу 0
  
  //radio.openWritingPipe(address[1]);     // хотим писать трубу 1
  radio.setChannel(CH_NUM);              // выбираем канал (в котором нет шумов!)
  radio.setPALevel(SIG_POWER);           // уровень мощности передатчика
  radio.setDataRate(SIG_SPEED);          // скорость обмена
  // должна быть одинакова на приёмнике и передатчике!
  // при самой низкой скорости имеем самую высокую чувствительность и дальность!!
  radio.powerUp();         // начать работу
  radio.startListening();  // начинаем слушать эфир, мы приёмный модуль
}



void handleSerialCommand(String command) {
  command.trim(); // Убирает лишние пробелы, \r и \n на всякий случай
 Serial.println(command);
  if (command == "10101") {
    Serial.println("Door open");
        for (int i = 0; i < 2; i++) {
        my_mux.channel(1);
        delay(2000);
        my_mux.channel(16);
        delay(2000);
      }
      myMP3.play(11);
  } else if (command == "10202") {
    Serial.println("goodbay");
    //doActionB();
  } 
   else if (command == "10303") {
    Serial.println("Fin");
    //doActionB();
  }
}
