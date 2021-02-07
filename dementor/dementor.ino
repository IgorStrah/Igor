#include <SoftwareSerial.h>
#include <Wire.h> 
#include <DFPlayerMini_Fast.h>
#include <IRremote.h>
#include <Stepper.h>
const int stepsPerRevolution = 200; 
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);
int RECV_PIN = A0;
SoftwareSerial mySerial(3, 2); // RX, TX
DFPlayerMini_Fast myMP3;
#include <IRremote.h>      
// 3 high, 1 high/PWM - rotation
// 2 high, 4 high/PWM - reverse
// 1 high, 4 high - brake
IRrecv irrecv(RECV_PIN);


// пины драйвера, P1 и P4 должны быть на PWM пинах! (3, 5, 6, 9, 10, 11 для NANO и UNO)
#define P1 5
#define P2 4
#define P3 7
#define P4 6

#define potPin 0      // сюда подключен потенциометр
const int buttonPin = A2;     // the number of the pushbutton pin
boolean switch_flag;
int potent, duty;
int left_min, right_min;

void setup() {
  // все пины драйвера как выходы, и сразу выключаем
  pinMode(P1, OUTPUT);
  digitalWrite(P1, 0);
  pinMode(P2, OUTPUT);
  digitalWrite(P2, 0);
  pinMode(P3, OUTPUT);
  digitalWrite(P3, 0);
  pinMode(P4, OUTPUT);
  digitalWrite(P4, 0);
  // Модуль шаговый двигатель
  myStepper.setSpeed(120);


  // модуль сериал. а вдруг
  
  Serial.begin(115200);
  mySerial.begin(9600);

 
  myMP3.begin(mySerial);

  Serial.println(F("Startup"));
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Setting volume to max");
  myMP3.volume(25);
  delay(20);
  pinMode(buttonPin, INPUT_PULLUP);
  //аdigitalWrite(pin, HIGH);




  
}

void loop() {
  
  if (digitalRead(buttonPin)==0) {
  
    // вырубить все ключи
    digitalWrite(P1, 0);
    digitalWrite(P2, 0);
    digitalWrite(P3, 0);
    digitalWrite(P4, 0);
    delay(15);  

  
      digitalWrite(P2, 0);    // вырубить Р канальный второго плеча
      digitalWrite(P4, 0);    // вырубить N канальный второго плеча
      delayMicroseconds(5);   // задержечка на переключение на всякий случай
      digitalWrite(P3, 1);    // врубить Р канальный первого плеча

  
      analogWrite(P1, 100);    // ЖАРИТЬ ШИМ!
      delay(1500);  
  
      digitalWrite(P3, 0);    // вырубить Р канальный первого плеча
      digitalWrite(P1, 0);    // вырубить N канальный первого плеча
      delayMicroseconds(5);   // задержечка на переключение на всякий случай
      digitalWrite(P2, 1);    // врубить Р канальный второго плеча
  
     analogWrite(P4, 100);    // ЖАРИТЬ ШИМ!
      delay(1500);
      digitalWrite(P1, 0);
    digitalWrite(P2, 0);
    digitalWrite(P3, 0);
    digitalWrite(P4, 0);
    delay(15);  

  
    myStepper.step(50);
    
  }

  

}
