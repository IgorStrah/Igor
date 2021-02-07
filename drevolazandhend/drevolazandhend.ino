
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
DFPlayerMini_Fast myMP3;
SoftwareSerial mySerial(A3, A4); // RX, TX
#include <IRremote.h>
int RECV_PIN = 3;
int counter = 0;
int counter2 = 0;
unsigned long irvalue;
uint32_t tmr;
uint32_t tmr2;
int  tmrup;


int stepcounter = 1;
boolean flag;
int start1 = 0;
int start2 = 0;
boolean stop1 = 0;
uint32_t tmrdown;

int analogOutPin = 9;
int handup = 6;
int handdown = 5;
int dartfrogup = 7;
int dartfrogdown = 8;
int who = 0; // whi is? hand ==0 dartfrog ==1
IRrecv irrecv(RECV_PIN);
decode_results results;

int date;


int rec;
#include "GyverBus.h"

#define RX_PIN 2    // пин
#define RX_ADDR 8   // наш адрес

// работает как отправитель и приёмник!! GBUS_FULL
GBus<RX_PIN, GBUS_FULL> rx(RX_ADDR, 15);



// Если значенике первого байт больше нуля - запускается программа.
// счётчик вычитает значени. Если значения дойдт до "0" программа останавливается с проигрышем
// если приходит по ИК коду верное значение, программа останавливается с выигришем. передаётся второй байт "1" и обнуляется первый байт



void setup() {

 Serial.begin(9600);
  Serial.println("Hello Kitty!");
  Serial.print("Мяу!");

  pinMode(handup, OUTPUT);
  pinMode(handdown, OUTPUT);
  pinMode(dartfrogdown, OUTPUT);
  pinMode(dartfrogup, OUTPUT);
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
    counter = rec * 10 ; // получаем счётчик на 1/10 секунды
    tmrdown=rec;
    //   - разово включаем звук. надо сделать его на подольше.


    start1 = 1;
    who == 0 ? who = 1 : who = 0;


    tmrup = 0;
    stepcounter = 1;

  }

  // приняли запрос request
  if (rx.gotRequest()) {
    // отправили данные
    rx.sendData(rx.getTXaddress(), tmrdown);  // отправить дату
  }



 
//  Read date IRDA
  if (irrecv.decode(&results))   // Модуль ИК кода
  {
    irvalue = results.value;
    irrecv.resume(); // Receive the next value
   if ((irvalue == 1111000002)||(irvalue == 16726215)) // атака проведена верно  - сворачиваемся.
    
    {


    if (counter>2)
    {
      counter=3;
      start1=3;
      tmrdown=99;
     }
     }
  }
  




   if (who == 0)
    {

  if (counter > 2)
  {



    if (start1 == 1)
    {
      drevolazmov(1, 18);
      tmrup = 8;  start1 = 2;
    }

  }


  if (counter == 2)
  {
 
    if (start1 == 3)
    {
      drevolazmov(2, 90);   tmrup = 8;   start1 = 2;
    
    }
  }





  if  ((tmrup == 1) && (start1 == 2))
  {
    drevolazmov(0, 0);   start1 = 3;
  }


  }

else
{


 if (counter > 2)
  {
      drevolazmov(3, 125);
   }


  if (counter == 2)
  {
 drevolazmov(0, 0);
  }

}


  if (millis() - tmr >= 100) // каждые 1/10 сек
  {
    tmr = millis();
    if  (counter > 1) {
      counter--;
    }
  }

  if (millis() - tmr2 >= 1000) // каждые 1/10 сек
  {
    tmr2 = millis();
    if  (tmrdown>1)  tmrdown--;
    if  (tmrup > 1) {
      tmrup--;
    }

  }
}


int drevolazmov(int direct, int power)
{

  if (direct == 0)
  {
    // hand up
    analogWrite(analogOutPin, 0);
    digitalWrite(handup, LOW);
    digitalWrite(handdown, LOW);
    digitalWrite(dartfrogup, LOW);
    digitalWrite(dartfrogdown, LOW);
  }
  else  if (direct == 1)  // UP
  {
    analogWrite(analogOutPin, power);
    digitalWrite(dartfrogup, LOW);
    digitalWrite(dartfrogdown, HIGH);

  }
  else  if (direct == 2)  // DOWN
  {
    analogWrite(analogOutPin, power);
    digitalWrite(dartfrogup, HIGH);
    digitalWrite(dartfrogdown, LOW);

  }
  else  if (direct == 3)  // DOWN
  {
    analogWrite(analogOutPin, power);
    digitalWrite(handup, HIGH);
    digitalWrite(handdown, LOW);

  }
  else  if (direct == 4)  // DOWN
  {
    analogWrite(analogOutPin, power);
    digitalWrite(handup, HIGH);
    digitalWrite(handdown, LOW);

  }

}
