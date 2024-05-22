#include <Arduino.h>
#include <SoftwareSerial.h>
// Initialise on software serial port.
SoftwareSerial SoftSerial(2, 3);
#include "DYPlayerArduino.h"
DY::Player player(&SoftSerial);


const int correctSequence[] = { 6, 4, 11, 7, 5, 10, 8, 9 };  // Правильная последовательность
const int sequenceLength = sizeof(correctSequence) / sizeof(correctSequence[0]);
int currentIndex = 0;              // Текущий индекс в последовательности
bool pinActivated[9] = { false };  // Флаги активации для каждого пина
bool pinState[9] = { true };       // Текущее состояние каждого пина (true, если кнопка отпущена)

int counter = 0;       // замена i
uint32_t timer = 0;    // переменная таймера
#define T_PERIOD 2000  // период переключения

void setup() {
  Serial.begin(115200);  // Инициализация последовательного порта со скоростью 9600 бод
  for (int pin = 4; pin <= 12; pin++) {
    pinMode(pin, INPUT_PULLUP);  // Настройка пинов как входов с подтяжкой к питанию
  }
  pinMode(13, OUTPUT);
 
  digitalWrite(13, LOW);

 
  player.begin();
  player.setVolume(10);  // 50% Volume
  //player.PlayMode(OneOff);//setCycleMode(DY::PlayMode::Repeat); // Play all and repeat.
  player.setCycleMode(DY::PlayMode::OneOff);
  player.setVolume(12);  // 50% Volume
}

void loop() {

  if (digitalRead(12) == HIGH) {
     digitalWrite(13, HIGH);
    if (millis() - timer >= T_PERIOD) {  // таймер на millis()
      timer = millis();                  // сброс
      // действие с counter - наш i-ый светодиод например

      bool switchit = 0;
      for (int pin = 4; pin <= 11; pin++) {
        if (digitalRead(pin) == LOW) {
          switchit = 1;
        }
      }
      if (switchit == 0) {
        // прибавляем счётчик
        if (counter > 8) counter = 0;
        player.playSpecified((correctSequence[counter]) - 3);
        Serial.println((correctSequence[counter]) - 3);
        counter++;
        delay(1200);
      } else {
        counter = 0;
      }
    }



    for (int pin = 4; pin <= 11; pin++) {
      int pinIndex = pin - 4;  // Индекс в массиве флагов
      bool currentPinState = digitalRead(pin) == LOW;
      if (currentPinState && !pinActivated[pinIndex]) {
        pinActivated[pinIndex] = true;  // Установка флага активации
        Serial.print("Кнопка на пине ");
        Serial.print(pin);
        Serial.println(" активирована.");
        if (pin == correctSequence[currentIndex]) {
          currentIndex++;
          player.playSpecified(pinIndex + 1);
          delay(900);
          if (currentIndex == sequenceLength) {
            digitalWrite(13, LOW);

            while(1)
            {
                if  (digitalRead(4) == LOW)
                {
                digitalWrite(13, LOW);
                }
                else {
                 digitalWrite(13, HIGH);
                }
            }
            
           
          }
        } else {
          Serial.println("Ошибка в последовательности. Начните сначала.");
          resetActivationFlags();
          currentIndex = 0;
          player.playSpecified(9);
          delay(1200);
          break;
        }
      } else if (!currentPinState && pinActivated[pinIndex] && pinState[pinIndex]) {
        player.playSpecified(9);
        delay(1200);
        Serial.println("Кнопка была отпущена. Ошибка. Начните сначала.");
        resetActivationFlags();
        currentIndex = 0;
        break;
      }
      pinState[pinIndex] = currentPinState;  // Обновление состояния пина
    }
  }
}

void resetActivationFlags() {
  for (int i = 0; i < 9; i++) {
    pinActivated[i] = false;  // Сброс всех флагов активации
    pinState[i] = true;       // Сброс состояния кнопок на "отпущено"
  }
}
