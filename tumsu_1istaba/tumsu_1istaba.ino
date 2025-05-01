#include <HCSR04.h>

#include <SoftwareSerial.h>
// Initialise on software serial port.
SoftwareSerial SoftSerial(3, 4);
#include "DYPlayerArduino.h"
DY::Player player(&SoftSerial);


int buttonState = 0;      // Текущее состояние кнопки
int lastButtonState = 0;  // Предыдущее состояние кнопки
int ledState = LOW;       // Текущее состояние светодиода
int num_sound = 8;        // Текущее состояние светодиода

int alarm;
byte triggerPin = 13;
byte echoCount = 4;
byte step = 0;
byte* echoPins = new byte[echoCount]{ 12, 11, 10, 9 };
byte noundnom = 2;
byte stepclock;
const int buttonPin = A0;
void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(A1, OUTPUT);
  digitalWrite(A1, LOW);
  pinMode(A2, OUTPUT);
  digitalWrite(A2, HIGH);
  pinMode(A3, INPUT_PULLUP);
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  pinMode(A5, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(A4, INPUT);
  pinMode(7, INPUT_PULLUP);


  Serial.begin(115200);
  HCSR04.begin(triggerPin, echoPins, echoCount);
  Serial.print("Start ");

  player.begin();
  player.setVolume(20);  // 50% Volume
  delay(111);
  //player.PlayMode(OneOff);//setCycleMode(DY::PlayMode::Repeat); // Play all and repeat.
  player.setCycleMode(DY::PlayMode::RepeatOne);
  delay(111);
  player.setVolume(20);  // 50% Volume
  delay(111);
  playsoundnom(2);
  delay(111);
}

void loop() {

  if (step == 0) {
    // music
    if (digitalRead(7) == 1) {
      if (num_sound <= 10) {
        num_sound++;
      } else {
        num_sound = 2;
      }
      playsoundnom(num_sound);
      delay(5000);
      player.stop();
    }

    double* distances = HCSR04.measureDistanceCm();
    for (int i = 0; i < echoCount; i++) {
      Serial.print("sensor: ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(distances[i]);
      Serial.println(" cm");
    }

    if ((distances[0] < 58 || distances[1] < 55 || distances[3] < 60)&&()) {
      alarm = 1;
      stepclock=0;
      playsoundnom(1);
    }


    Serial.println("");
    Serial.print("alarm ");
    Serial.println(alarm);
    Serial.println("---");
    Serial.println(digitalRead(buttonPin));

    if (alarm == 0 && distances[2] > 100 && digitalRead(A3) == 0) {
      stepclock++;
      if (stepclock==3)
      {
      digitalWrite(A2, LOW);
      step = 1;
      playsoundnom(1);
      }
    }
    
      if (digitalRead(buttonPin) == 0) {
      alarm = 0;
      stepclock=0;
      Serial.println("");
      Serial.print("alarm ------------------2 ");
      Serial.println(alarm);
      playsoundnom(2);
    }
    
    delay(250);

  }



  else if (step == 1) {
    playsoundnom(222);
    if (digitalRead(A5) == 0) {
      step = 2;
      digitalWrite(A2, HIGH);
    }
  } else if (step == 2) {
    digitalWrite(A1, HIGH);
    digitalWrite(6, LOW);

    if (digitalRead(7) == LOW) {
      digitalWrite(A2, HIGH);
    } else {
      digitalWrite(A2, LOW);
    }



    if (digitalRead(A4) == 0)  // Сработал звук.
    {
      playsoundnom(num_sound);
      Serial.println("");
      Serial.print("sound  ");
      delay(1000);
      while (digitalRead(5) == 1) {
          playsoundnom(num_sound);
        digitalWrite(A1, LOW);
        if (digitalRead(7) == LOW) {
          digitalWrite(A2, HIGH);
        } else {
          digitalWrite(A2, LOW);
        }
      }
      playsoundnom(222);
      delay(1000);
    }
  }
}

void playsoundnom(byte soundnomnow) {
  if (noundnom != soundnomnow) {

    player.playSpecified(soundnomnow);
    if (soundnomnow == 222) { player.stop(); }
    noundnom = soundnomnow;
  }
}
