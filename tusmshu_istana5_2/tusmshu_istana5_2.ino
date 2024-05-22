#include <Arduino.h>
#include <SoftwareSerial.h>
// Initialise on software serial port.
SoftwareSerial SoftSerial(2, 3);
#include "DYPlayerArduino.h"
DY::Player player(&SoftSerial);


// Пины кнопок
const int buttonPins[] = {4, 5, 6, 7, 8};
// Пин открытия замка
const int lockPin = A0;
// Правильная последовательность нажатий на кнопки
const int correctCode[] = { 0, 1, 2, 3};
const int codeLength = 4;  // Длина кода
int Code[] = {0,0,0,0};
int buttonState[5] = {};
int previousButtonState;
int currentIndex = 0;  // Текущий индекс для добавления нового значения
int noundnom;

void setup() {
  // Настройка пина открытия замка
  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, HIGH); 
  // Настройка последовательного порта
  Serial.begin(115200);
  Serial.println("Кодовый замок инициализирован.");

  // Настройка пинов кнопок
  for (int i = 0; i < 5; i++) {
    pinMode(buttonPins[i], INPUT);
  }

  
  player.begin();
  player.setVolume(20);  // 50% Volume
  //player.PlayMode(OneOff);//setCycleMode(DY::PlayMode::Repeat); // Play all and repeat.
  player.setCycleMode(DY::PlayMode::OneOff);
  player.setVolume(20);  // 50% Volume
}

void loop() {
  int enteredCode[codeLength];
  int enteredCodeIndex = 0;

  // Чтение состояния кнопок и запись нажатых кнопок в массив
  for (int i = 0; i < 5; i++) {
    buttonState[i] = digitalRead(buttonPins[i]);
    if (buttonState[i] == HIGH && previousButtonState != i) {
      enteredCode[enteredCodeIndex] = i;
      enteredCodeIndex++;
      addData(i);
      // Serial.print("previousButtonState[i] != buttonState[i]  ");
      // Serial.print(buttonState[i]);
      // Serial.print("    ");
      Serial.println("play");
      Serial.print("    ");
      Serial.print(i+1);
        playsoundnom(i+1);
      delay(50);  // Для устранения дребезга
      previousButtonState = i;
      printArray() ;
    }
  }


  bool codeCorrect = true;
  for (int i = 0; i < codeLength; i++) {
    // Serial.print(" Code[i]   ");
    // Serial.print(Code[i]);
    // Serial.print(" correctCode[i]   ");
    // Serial.print(correctCode[i]);

    if (Code[i] != correctCode[i]) {
      codeCorrect = false;
      break;
    }
  }
 // Serial.println();
  // Если код правильный, открыть замок
  if (codeCorrect) {
    
    delay(2000);  // Подать импульс на 1 секунду
    unlock();
    Serial.println("Замок открыт.");
    
    for (int i = 0; i < codeLength; i++) {
      Code[i - 1] = 0;
    }
  }
}

void addData(int data) {

    // Если массив полностью заполнен, сдвигаем значения
    for (int i = 0; i < codeLength; i++) {
      Code[i - 1] = Code[i];
    }
    Code[codeLength-1] = data;  // Добавляем новое значение в конец массива
  
}
void printArray() {
  for (int i = 0; i < codeLength; i++) {
     
    Serial.print("   i");
    Serial.print(i);
    Serial.print("  ");
    Serial.print(Code[i]);
  }
  Serial.println();
}


void unlock() {
  digitalWrite(lockPin, LOW);  // Подать импульс на пин открытия замка
playsoundnom(222);

  
}

void playsoundnom(byte soundnomnow) {
  if (noundnom != soundnomnow) {

    player.playSpecified(soundnomnow);
    if (soundnomnow == 222) { player.stop(); }
    noundnom = soundnomnow;
  }
}
