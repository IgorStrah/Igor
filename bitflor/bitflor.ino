#include <Servo.h>

Servo bitbox;  // create servo object to control a servo
// twelve servo objects can be created on most boards
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
DFPlayerMini_Fast myMP3;
SoftwareSerial mySerial(A1, A0); // RX, TX
#include <IRremote.h>
int RECV_PIN = 3;
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
int pos = 0;
IRrecv irrecv(RECV_PIN);
decode_results results;

int date;


int rec;
#include "GyverBus.h"

#define RX_PIN 2    // пин
#define RX_ADDR 9   // наш адрес

// работает как отправитель и приёмник!! GBUS_FULL
GBus<RX_PIN, GBUS_FULL> rx(RX_ADDR, 15);


// Если значенике первого байт больше нуля - запускается программа.
// счётчик вычитает значени. Если значения дойдт до "0" программа останавливается с проигрышем
// если приходит по ИК коду верное значение, программа останавливается с выигришем. передаётся второй байт "1" и обнуляется первый байт


void setup() {

  // bitbox.attach(9);  // attaches the servo on pin 9 to the servo object
  irrecv.enableIRIn(); // Start the receiver

  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(25);
  delay(20);

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
    counter = rec * 500; // получаем счётчик на 1/10 секунды
    tmrdown = rec;
    //   - разово включаем звук. надо сделать его на подольше.

    tmrup = 0;
    bitbox.attach(9);  // attaches the servo on pin 9 to the servo object
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
    if ((irvalue == 1111000002) || (irvalue == 16726215)) // атака проведена верно  - сворачиваемся.
    {

      if (counter > 2)
      {
        counter = 2;

        tmrdown = 99;
      }

    }
  }



  if (counter == 0)
  {
    bitbox.detach();
  }




  if (millis() - tmr2 >= 2) // каждые 1/10 сек
  {
    tmr2 = millis();
    if  (counter > 0) {
      counter--;
    }
    if   (counter > 0)
    {
      if ((pos < 180) && (flag != 1))
      {
        pos++;
        if (pos == 180)
        {
          flag = 1;
        }
      }
      else
      {
        pos--;
        if (pos == 1)
        {
          flag = 0;
        }
      }
      bitbox.write(pos);              // tell servo to go to position in variable 'pos'
    }
  }



  if (millis() - tmr >= 1000) // каждые 1/10 сек
  {
    tmr = millis();
  if  (tmrdown > 0) 
    {
      tmrdown--;
    }

  }

}
