
#include <SPI.h>;       // Библиотека для работы с шиной SPI
#include <nRF24L01.h>;  // Файл конфигурации для библиотеки RF24
#include <RF24.h>;      // Библиотека для работы с модулем NRF24L01
#include <IRremote.h>
#define PIN_CE 7   // Номер пина Arduino, к которому подключен вывод CE радиомодуля
#define PIN_CSN 8  // Номер пина Arduino, к которому подключен вывод CSN радиомодуля
#define DEBUG
RF24 radio(PIN_CE, PIN_CSN);  // Создаём объект radio с указанием выводов CE и CSN
uint32_t newCode[2];

const int buttonPins[] = { 4, 5, 6 };  // Массив пинов кнопок
const int numButtons = 3;              // Количество кнопок

unsigned long lastPressTime = 0;
const unsigned long interval = 3000;      // 3 секунды для нажатий
const unsigned long waitInterval = 5000;  // 5 секунд ожидания следующего нажатия

int sequenceIndex = 0;
int repeatCount = 0;
byte sword = 0;

byte mode;  // - 0 = only magnitm, 1= alohomore 2= sword + alohomore
void setup() {
#ifdef DEBUG
  Serial.begin(115200);  // Инициализация серийного порта для отладки
#endif
  IrReceiver.begin(2);
  radio.begin();                          // Инициализация радиомодуля NRF24L01
  radio.setChannel(5);                    // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setDataRate(RF24_1MBPS);          // Скорость обмена данными 1 Мбит/сек
  radio.setPALevel(RF24_PA_HIGH);         // Выбираем высокую мощность передатчика (-6dBm)
  radio.openWritingPipe(0x7878787878LL);  // Открываем трубу с уникальным ID
  mode = 0;
  pinMode(3, OUTPUT);
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT);
  }
}

void loop() {



if (mode!=2) {
  if (sequenceIndex % 2 == 0) {
   
    if (digitalRead(buttonPins[sequenceIndex / 2]) == HIGH) {
      waitForRelease();
      #ifdef DEBUG
        Serial.print("pin ");
      Serial.println(sequenceIndex / 2);
      #endif
    }
  } else {

    if (digitalRead(buttonPins[sequenceIndex / 2]) == LOW) {
      nextButton();
        #ifdef DEBUG
        Serial.print("pin ");
      Serial.println(sequenceIndex / 2);
      #endif
    }
  }

  if (millis() - lastPressTime > interval) {
      Serial.print("lastPressTime ");
      Serial.println(millis() - lastPressTime);
    resetSequence();
  }
}



  if (IrReceiver.decode()) {
    unsigned long irValue = IrReceiver.decodedIRData.decodedRawData;  // 

    for (int i = 0; i < 32; i++) {
      // Extract the ith bit from the old code
      uint32_t bit = (irValue >> (31 - i)) & 1;

      // Set the ith bit in the new code
      newCode[0] |= (bit << i);
    }
    if (newCode[0] != 0) {
//  newCode[1]=101;
#ifdef DEBUG

      Serial.println(newCode[0]);
#endif
    }

    if (newCode[0] == 16736925) {
      (mode == 2) ? mode = 0 : mode++;

      digitalWrite(3, HIGH);     // turn the LED on (HIGH is the voltage level)
      delay(500 + mode * 2000);  // wait for a second
      digitalWrite(3, LOW);      // turn the LED off by making the voltage LOW


#ifdef DEBUG
      Serial.print("mode ");
      Serial.println(mode);
#endif
    }

    if ((newCode[0] == 1111000005) || (newCode[0] == 16726215)) {
      if (mode==1&&sword==1)
      {
      radio.write(newCode, sizeof(newCode));  // Передаём данные по радиоканалу
      }
    } else if ((newCode[0] == 1111000006) || (newCode[0] == 16734885)) {
      if (mode==2&&sword==1)
      {
      radio.write(newCode, sizeof(newCode));  // Передаём данные по радиоканалу
      }
    }
    IrReceiver.resume();
    newCode[0] = 0;
  }
}





void waitForRelease() {
  unsigned long startTime = millis();
  while (millis() - startTime < interval) {
    // Проверяем, что кнопка отпущена до истечения времени
    if (digitalRead(buttonPins[sequenceIndex / 2]) == LOW) {
      nextButton();
      return;
    }
  }
  // Если кнопка не была отпущена в течение 3 секунд, сбрасываем последовательность
  resetSequence();
}

void nextButton() {
  lastPressTime = millis();
  sequenceIndex++;
  #ifdef DEBUG
          Serial.print("sequenceIndex ");
      Serial.println(sequenceIndex);
      #endif
  if (sequenceIndex == numButtons * 2) {
    repeatCount++;
    if (repeatCount >= 2) {
      Serial.println("sword");
       digitalWrite(3, HIGH);  
      sword++;
      resetSequence();
    } else {
      sequenceIndex = 0;
    }
  }
}

void resetSequence() {
  sequenceIndex = 0;
  repeatCount = 0;
  lastPressTime = millis();
  Serial.println("Сброс последовательности");
}