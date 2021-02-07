#include "Servo.h"
#include <SoftwareSerial.h>

#include <DFPlayerMini_Fast.h>
SoftwareSerial mySerial(3, 2); // RX, TX
DFPlayerMini_Fast myMP3;
#include <IRremote.h>

int RECV_PIN = A2;


#include "GyverBus.h"

#define RX_PIN 10    // пин
#define RX_ADDR 3   // наш адрес

// работает как отправитель и приёмник!! GBUS_FULL
GBus<RX_PIN, GBUS_FULL> rx(RX_ADDR, 15);


#define HEAD_PIN    9 // 110 раскрыто 0 закрыто
#define RIGHT_HAND_PIN    5 // 110 раскрыто 0 закрыто
#define LEFT_HAND_PIN    6 // 170 раскрыто 60 закрыто

IRrecv irrecv(RECV_PIN);
decode_results results;

Servo HEAD, LEFT_HAND, RIGHT_HAND; // create servo object to control a servo
int HEAD_val = 150;
int LEFT_HAND_val = 70;
int RIGHT_HAND_val = 100;

int counter = 0;
int counterclose = 0;
int cloc;
uint32_t tmr;
uint32_t tmr2;
uint32_t tmrup;
int tmrdown;
int startserv = 0;
int openclose = 0;
int rec;

// Если значенике первого байт больше нуля - запускается программа.
// счётчик вычитает значени. Если значения дойдт до "0" программа останавливается с проигрышем
// если приходит по ИК коду верное значение, программа останавливается с выигришем. передаётся второй байт "1" и обнуляется первый байт
// переменная таймера, максимально большой целочисленный тип (он же uint32_t)

unsigned long irvalue;

void setup() {

  Serial.begin(9600);
  mySerial.begin(9600);
  myMP3.begin(mySerial);

  irrecv.enableIRIn(); // Start the receiver

  myMP3.volume(25);
  delay(20);



}


void loop() {




  rx.tick();

  if (rx.gotData()) {
    rx.readData(rec);
    counter = rec * 100; // получаем счётчик на 1/10 секунды
    //   - разово включаем звук. надо сделать его на подольше.
    //myMP3.reset();
    myMP3.play(3);
    myMP3.volume(15);
    
    tmrup = 0;
    tmrdown = rec;
    
    digitalWrite(LED_BUILTIN, HIGH);
    tmrdown=rec;
  }
 
   // приняли запрос request
  if (rx.gotRequest()) {
      // отправили данные
    rx.sendData(rx.getTXaddress(), tmrdown);  // отправить дату
    }



  if (counter > 2)
  {
    
    
    openclose = 1;
    HEAD.attach(HEAD_PIN);  // attaches the servo on pin 9 to the servo object
    LEFT_HAND.attach(LEFT_HAND_PIN);  // attaches the servo on pin 9 to the servo object
    RIGHT_HAND.attach(RIGHT_HAND_PIN);  // attaches the servo on pin 9 to the servo object




 //  Read date IRDA
  if (irrecv.decode(&results))   // Модуль ИК кода
  {
    irvalue = results.value;
     Serial.println(irvalue);
    irrecv.resume(); // Receive the next value
   if ((irvalue == 1111000003) || (irvalue == 16726215)) // атака проведена верно  - сворачиваемся.
    {
      
 
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    openclose = 0;
    counterclose = 400;
    myMP3.wakeUp();
    myMP3.play(6);
    myMP3.volume(25);
    counter=2;

    
      tmrdown=99;
     }
  }
  }
  else if (counter == 2)
  {
    //позиция на закрытие
    /*
      if (startserv == 1)
      {
      // позиция на открытие

      HEAD_val = 40;
      LEFT_HAND_val = 140;
      RIGHT_HAND_val = 5;
      startserv = 0;

      }
    */
    openclose = 0;
    counterclose = 400;
    
    
   // if (tmrdown=!99)
   // {
      myMP3.wakeUp();
      delay(5);
      myMP3.play(10);
      myMP3.volume(20);
   // }



    
    delay(50);
  }

  if (millis() - tmr >= 10) // каждые 1/10 сек
  {
    tmr = millis();

    if  (counter > 1) counter--;
    if  (counterclose > 1) counterclose--;

    tmrup++;



    if ((openclose == 0) && (counterclose <= 2))
    {
      HEAD.detach();
      RIGHT_HAND.detach();
      LEFT_HAND.detach();

    }
    else
    {
      if (tmrup >= 3)
      {
        OpenClose(openclose);
        HEAD.write(HEAD_val);
        RIGHT_HAND.write(RIGHT_HAND_val);
        LEFT_HAND.write(LEFT_HAND_val);
        tmrup = 0;
      }
    }
  }

}

int OpenClose(int flag)
{
  if (flag == 0)
  {
    // close
   
    if (80 <= LEFT_HAND_val)
    {
      LEFT_HAND_val--;
    }
    if (155 >= RIGHT_HAND_val)
    {
      RIGHT_HAND_val++;
    }
      if (120 >= HEAD_val)
    {
      HEAD_val++;
    }
  }

  else
  {
    // open
   
    if (155 >= LEFT_HAND_val)
    {
      LEFT_HAND_val++;
    }
    if (15 <= RIGHT_HAND_val)
    {
      RIGHT_HAND_val--;
    }
     if (10 <= HEAD_val)
    {
      HEAD_val--;
    }

  }

    if (millis() - tmr2 >= 1000) // каждые 1/10 сек
  {
   tmr2 = millis();
   if  (counter>2) 
   tmrdown--;
  }
}
