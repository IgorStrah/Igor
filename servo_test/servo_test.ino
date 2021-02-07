#include <iarduino_I2C_connect.h>
#include "Servo.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <DFPlayerMini_Fast.h>
SoftwareSerial mySerial(3, 2); // RX, TX
DFPlayerMini_Fast myMP3;
#include <IRremote.h>

int RECV_PIN = A2;


#define RIGHT_HAND_PIN    5 // 110 раскрыто 0 закрыто
#define LEFT_HAND_PIN    6 // 170 раскрыто 60 закрыто
#define HEAD_PIN       9 // 0 - вверх 90 в низ

IRrecv irrecv(RECV_PIN);
decode_results results;

Servo HEAD, LEFT_HAND, RIGHT_HAND; // create servo object to control a servo
int HEAD_val = 150;
int LEFT_HAND_val = 70;
int RIGHT_HAND_val = 100;

int counter = 0;
int counterclose=0;
int cloc;
uint32_t tmr;
uint32_t tmrup;
int startserv = 0;
int openclose = 0;

iarduino_I2C_connect I2C2;                            // объявляем переменную для работы c библиотекой iarduino_I2C_connect
byte  REG_Massive[3];                  // Три значения. первый байт - сколько секунд работаем, вторйо байт - что приняли тот нужный ИК код ( равез 1 или 0), 3 - скорость с которой работаем.
// Если значенике первого байт больше нуля - запускается программа.
// счётчик вычитает значени. Если значения дойдт до "0" программа останавливается с проигрышем
// если приходит по ИК коду верное значение, программа останавливается с выигришем. передаётся второй байт "1" и обнуляется первый байт

unsigned long period_time = 50;
// переменная таймера, максимально большой целочисленный тип (он же uint32_t)
unsigned long my_timer;
unsigned long irvalue;

void setup() {

  Serial.begin(115200);
  mySerial.begin(9600);
  myMP3.begin(mySerial);


  irrecv.enableIRIn(); // Start the receiver

  myMP3.volume(25);
  delay(20);

  //HEAD.attach(HEAD_PIN);  // attaches the servo on pin 9 to the servo object
 // LEFT_HAND.attach(LEFT_HAND_PIN);  // attaches the servo on pin 9 to the servo object
 // RIGHT_HAND.attach(RIGHT_HAND_PIN);  // attaches the servo on pin 9 to the servo object
  //HEAD.write(100);
 // LEFT_HAND.write(20);
 // RIGHT_HAND.write(100);

  Wire.begin(0x03);     // инициируем подключение к шине I2C в качестве ведомого (slave) устройства, с указанием своего адреса на шине.
  I2C2.begin(REG_Massive);

}


void loop() {


  if (REG_Massive[0] > 1 ) // запуск движения, не цекличное.
  {

    counter = REG_Massive[0] * 100 ; // получаем счётчик на 1/10 секунды
    //   - разово включаем звук. надо сделать его на подольше.
      myMP3.wakeUp();
    myMP3.play(3);
    myMP3.volume(25);
    delay(50);
    REG_Massive[1] = 1;
    REG_Massive[0] = 0;
  
  }


  if (counter > 2)
  {
   /*
    if (startserv == 0)
    {
      // позиция на открытие
      HEAD_val = 150;
      LEFT_HAND_val = 0;
      RIGHT_HAND_val = 150;
      startserv = 1;
    }
   */
   openclose=1;
   HEAD.attach(HEAD_PIN);  // attaches the servo on pin 9 to the servo object
   LEFT_HAND.attach(LEFT_HAND_PIN);  // attaches the servo on pin 9 to the servo object
   RIGHT_HAND.attach(RIGHT_HAND_PIN);  // attaches the servo on pin 9 to the servo object
REG_Massive[1] = counter;
  }
  else if (counter== 2)
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
    counterclose=400;
      myMP3.wakeUp();
    myMP3.play(6);
    myMP3.volume(25);
    delay(50);
  }

  if (millis() - tmr >= 10) // каждые 1/10 сек
  {
    tmr = millis();
    
    if  (counter > 1) counter--;
    if  (counterclose > 1) counterclose--;
    
    tmrup++;
    
    
    
    if ((openclose==0)&&(counterclose<=2))
    {
    HEAD.detach();  
    RIGHT_HAND.detach(); 
    LEFT_HAND.detach(); 
    
    }
    else
    {
      if (tmrup>=3)
      {
    OpenClose(openclose);
    HEAD.write(HEAD_val);  
    RIGHT_HAND.write(RIGHT_HAND_val);
    LEFT_HAND.write(LEFT_HAND_val); 
    tmrup=0;
      }
    }
  }
 
}

int OpenClose(int flag)
{
if (flag==0)
{
  // close
  if (130 >= HEAD_val)
  { HEAD_val++;         }
  if (60 <= LEFT_HAND_val)
  { LEFT_HAND_val--;    }
  if (90 >= RIGHT_HAND_val)
  {  RIGHT_HAND_val++;  }
 
}

else
{
  // open
  if (30 <= HEAD_val)
  {   HEAD_val--;        }
  if (120 >= LEFT_HAND_val)
  { LEFT_HAND_val++;     }
  if (15 <= RIGHT_HAND_val)
  {   RIGHT_HAND_val--;  }
  
}
}
