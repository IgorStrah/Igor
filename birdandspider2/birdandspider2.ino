
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
DFPlayerMini_Fast myMP3;
SoftwareSerial mySerial(3, 4); // RX, TX
#include <IRremote.h>
int RECV_PIN = 1;
int counter = 0;
unsigned long irvalue;
uint32_t tmr;
uint32_t tmr2;
uint32_t tmrup;

int stepcounter = 1;
boolean flag;
int start1 = 0;
boolean start2 = 0;
boolean stop1 = 0;
uint32_t tmrdown;
int servostep = 0;
int analogOutPin = 9;
int snakeup = 6;
int snakedown = 5;
int spiderup = 7;
int spiderdown = 8;
int who = 0; // whi is? snake ==0 spider ==1
IRrecv irrecv(RECV_PIN);
decode_results results;

int date;


int rec;
#include "GyverBus.h"

#define RX_PIN 2    // пин
#define RX_ADDR 4   // наш адрес

// работает как отправитель и приёмник!! GBUS_FULL
GBus<RX_PIN, GBUS_FULL> rx(RX_ADDR, 15);



// Если значенике первого байт больше нуля - запускается программа.
// счётчик вычитает значени. Если значения дойдт до "0" программа останавливается с проигрышем
// если приходит по ИК коду верное значение, программа останавливается с выигришем. передаётся второй байт "1" и обнуляется первый байт


boolean switch_flag;
int potent, duty;
int left_min, right_min;

void setup() {

  
  pinMode(snakeup, OUTPUT);
  pinMode(snakedown, OUTPUT);
  pinMode(spiderdown, OUTPUT);
  pinMode(spiderup, OUTPUT);
  pinMode(analogOutPin, OUTPUT);


  irrecv.enableIRIn(); // Start the receiver

  
  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(25);
  delay(20);
  stepcounter = 1;
}

void loop() {

  // для начала ждём флаг от I2C.  флаг представляет 1 битное число, сколько секунд  мы ждём ИК сигнал от палочки ( из числа генерируем шим для поднятия вверх)!
  // включаем счётчик отсчёта!
  // ожидаем ИК сигнала от палочки. По приходу правильного сигнала выключаем звук и паркуем мандрагору в низ до pindown. -> Отправляем на мастер флаг что атака закончена.
  // поднимаемся вверх до срабатывания pinup и параллельно машем руками, и включаем звук на плеере ( жарим на всю)
  // Если/Когда поднялись - шевелим руками и по чуть чуть опускаемся вниз и опять поднимаемся вверх
  // Если по окончанию таймера не пришёл ИК код - паркуем мандрагору до pindown выключаем звук отправляем на мастер что атака не произведена


  //Проверим парковку - калибровку
  
rx.tick();
  
  if (rx.gotData()) {
     rx.readData(rec);
     counter = rec*10 ; // получаем счётчик на 1/10 секунды
   
    //   - разово включаем звук. надо сделать его на подольше.
 
 
    
    who==0 ? who=1:who=0;
    
  
    tmrup = 0;
    stepcounter = 1;
    
  }

  // приняли запрос request
  if (rx.gotRequest()) {
      // отправили данные
    rx.sendData(rx.getTXaddress(), stepcounter);  // отправить дату
    }


 
 

  if (counter >= 2)
  {


    // проверяем. если рандом = 0 . значит паук. если 1 - птица.

      if ((stepcounter == 1) && (tmrup < 75))
      {
        // spider down
        analogWrite(analogOutPin, 12);
        digitalWrite(spiderup, HIGH);
        digitalWrite(spiderdown, LOW);
       

      }
      else if ((stepcounter == 1) && (tmrup > 74))
      {
        stepcounter = 2; tmrup = 0; 

      }

      if  ((stepcounter == 2) && (tmrup < 100))
      {
        analogWrite(analogOutPin, 15);
        digitalWrite(spiderup, LOW);
        digitalWrite(spiderdown, LOW);
      
      }
    }
 
  else
  {

 
    if ((stepcounter == 2))
      {
        stepcounter = 3; tmrup = 0;  
      }

      if ((stepcounter == 3) && (tmrup < 40))
      {
        // spider up
        analogWrite(analogOutPin, 80);
        digitalWrite(spiderup, LOW);
        digitalWrite(spiderdown, HIGH);
      }
      else if ((stepcounter == 3) && (tmrup > 39))
      {
        stepcounter = 4; tmrup = 0;   date=5;

      }

      if  ((stepcounter == 4) && (tmrup < 40))
      {
        analogWrite(analogOutPin, 40);
        digitalWrite(spiderup, LOW);
        digitalWrite(spiderdown, HIGH);

      }

      else if ((stepcounter == 4) && (tmrup > 39))
      {
        stepcounter = 5; tmrup = 0; 
      }
      if  (stepcounter == 5)
    {
        analogWrite(analogOutPin, 0);
        digitalWrite(spiderup, LOW);
        digitalWrite(spiderdown, LOW);
        stepcounter = 0;
        tmrup = 0;
      }

      
    }
  


  if (millis() - tmr >= 100) // каждые 1/10 сек
  {
    tmr = millis();
    if  (counter > 1) {counter--;}
    
    tmrup++;
   
  }

}
