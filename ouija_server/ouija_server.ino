#define DEBUG

#include <CD74HC4067.h>
CD74HC4067 my_mux(2, 3, 4, 5);   
const int g_common_pin = 6;  
#include <MemoryFree.h> 

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
#define MAX_WORD_LENGTH 50
RF24 radio(9, 8);  // "создать" модуль на пинах 9 и 10 для НАНО/УНО
iarduino_IR_TX VD(3);    
//--------------------- БИБЛИОТЕКИ ----------------------

//--------------------- ПЕРЕМЕННЫЕ ----------------------
byte pipeNo;
byte address[][6] = { "1Node", "2Node", "3Node", "4Node", "5Node", "6Node" };  // возможные номера труб

struct Step {
  const char* word;
  void (*procedure)();
  const char* expectedUID;
};

// Переменные для логики игры
char targetWord[MAX_WORD_LENGTH] = "";
int currentLetterIndex = 0, currentLetterClock = 0;
bool gameActive = false;
unsigned long lastReadTime = 0;

byte tempclocll;
int telemetry[2];  // массив данных телеметрии (то что шлём на передатчик)
//--------------------- ПЕРЕМЕННЫЕ ----------------------
int x;

void procedure1() {
   
     digitalWrite(g_common_pin, LOW);  
        my_mux.channel(0);
        delay(1000);
         digitalWrite(g_common_pin, LOW);  
}

void procedure2() {
     digitalWrite(g_common_pin, HIGH);  
        my_mux.channel(1);
        delay(1000);
         digitalWrite(g_common_pin, LOW);  
} 

Step steps[] = {
  { "@clock1645", procedure1, "79046FE4" }, 
    { "@dorcnock6", procedure2, "FF0F34FC020000" }, 
      { "@page85", procedure2, "FF0F34FC020000" }, 
        { "@page85", procedure1, "FF0F34FC020000" }, 
          { "@page85", procedure1, "FF0F34FC020000" }, 
            { "@page85", procedure1, "FF0F34FC020000" }, 
              { "@page85", procedure1, "FF0F34FC020000" }, 
  { "@dimlight", procedure2, "FF0F60E9020000" }
};
const int stepCount = sizeof(steps) / sizeof(steps[0]);

int currentStep = 0;
bool waitingForWord = true;
bool waitingForWordwite = false;
bool waitingForUID = false;
String expectedUID = "";





void setup() {
    pinMode(g_common_pin, OUTPUT); 
    VD.begin();     
  Wire.begin();
  Serial.begin(115200);
  radioSetup();
  Serial.println("Start");
  Serial.print("Searching for word: ");
  Serial.println(steps[currentStep].word);
  expectedUID = searchInEEPROM(steps[currentStep].word[currentLetterIndex]);
      digitalWrite(g_common_pin, HIGH);
        my_mux.channel(0);
}

void loop() {

  if (waitingForWord) {
    Serial.print("Next letter: ");
    Serial.println(steps[currentStep].word[currentLetterIndex]);
    expectedUID = searchInEEPROM(steps[currentStep].word[currentLetterIndex]);
    waitingForWord = false;
  }



  // // Проверяем правильность буквы и задержку
  // if ( millis() - lastReadTime > 15000) {
  //   lastReadTime = millis();  // Обновляем таймер
  // Serial.println("✅ сенд труба2 буква!");
  // tempclocll++;
  //  char message[32];// = "Data from server" ;
  // sprintf(message, "Data from server %d", tempclocll);
  // radio.stopListening();  // Останавливаем прослушивание
  // delay(5);
  // radio.write(&message, sizeof(message));  // Отправляем данные на трубу 2
  // delay(5);
  // radio.startListening();  // Возвращаемся к прослушиванию
  // }


  if (radio.available(&pipeNo)) {  // слушаем эфир
    char receivedUID[16] = "";
    telemetry[0] = 0;
    telemetry[1] = 0;
    radio.read(&receivedUID, sizeof(receivedUID));

#ifdef DEBUG

  Serial.print("Free memory: ");
  Serial.println(freeMemory());  // Печатает количество свободной памяти

    Serial.print("Получено с pipe ");
    Serial.print(pipeNo);  // Выводим номер канала
    Serial.print("📤 UID  серверу: ");
    Serial.println(receivedUID);


    Serial.print("🔎 wait  UID: ");
    Serial.println(expectedUID);





#endif

if (strcmp(receivedUID, "FF0F38D5020000") == 0) 
{     digitalWrite(g_common_pin, HIGH);  
        my_mux.channel(1);
        delay(1000);
         digitalWrite(g_common_pin, LOW);  
             Serial.print("🔎 wait  UID: ");
    Serial.println("Open! ~~~~~~~~~~~~~~~~");
         }


    if (expectedUID == receivedUID) {

#ifdef DEBUG
      Serial.println("✅ Correct UID detected!");
#endif
      currentLetterClock++;
      telemetry[0] = 15 + currentLetterClock * 2;
      telemetry[1] = 1122;


      if (currentLetterClock == 10) {
        currentLetterClock = 0;
        currentLetterIndex++;
        waitingForWordwite=true;
      }


      if (steps[currentStep].word[currentLetterIndex] == '\0'&&waitingForUID==false) {  // Проверка конца слова
        Serial.println("✅ Word completed!");
        steps[currentStep].procedure();  // Выполняем процедуру
        waitingForUID = true;            // Ждём новый UID
        waitingForWord = false;
        
        
      } else {
        if ( waitingForWordwite==true) {
        waitingForWord = true;  // Переход к следующей букве
        waitingForWordwite=false;
        }
      }

    } else if (waitingForUID && strcmp(receivedUID, steps[currentStep].expectedUID) == 0) {
        Serial.print("✅ Expected UID received  ");
        Serial.println(steps[currentStep].expectedUID);
      currentStep++;
      currentLetterIndex = 0;
      waitingForUID = false;
      waitingForWord = true;
      telemetry[0] = 40;
      telemetry[1] = 1000;
      if (currentStep >= stepCount) {
        Serial.println("🎉 All steps completed!");
        while (true)
          ;
      }
    } else {

      currentLetterClock = 0;
      telemetry[0] = 1;
      telemetry[1] = 1;
#ifdef DEBUG
      Serial.println("❌ Incorrect UID, waiting for the correct one.");
#endif
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
    Wire.requestFrom(EEPROM_ADDR, 17);  // Запрашиваем 17 байт (UID + буква)


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
      return String(uidBuffer);
    }
  }

  return "";  // Если не нашли совпадение
}



void radioSetup() {                      // настройка радио
  radio.begin();                         // активировать модуль
  radio.setAutoAck(1);                   // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);               // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();              // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);              // размер пакета, байт
  radio.openReadingPipe(1, address[0]);  // хотим слушать трубу 0
  radio.openWritingPipe(address[1]);     // хотим писать трубу 1
  radio.setChannel(CH_NUM);              // выбираем канал (в котором нет шумов!)
  radio.setPALevel(SIG_POWER);           // уровень мощности передатчика
  radio.setDataRate(SIG_SPEED);          // скорость обмена
  // должна быть одинакова на приёмнике и передатчике!
  // при самой низкой скорости имеем самую высокую чувствительность и дальность!!
  radio.powerUp();         // начать работу
  radio.startListening();  // начинаем слушать эфир, мы приёмный модуль
}
