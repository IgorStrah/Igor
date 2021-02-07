// twelve servo objects can be created on most boards
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
DFPlayerMini_Fast myMP3;
SoftwareSerial mySerial(A1, A2); // RX, TX
#include <IRremote.h>
int RECV_PIN = 3;
int counter = 0;
unsigned long irvalue;
uint32_t tmr;
uint32_t tmr2;
uint32_t tmrup;
int targetdata;
int stepcounter = 1;
boolean flag = 1;
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
#define RX_ADDR 7   // наш адрес

// работает как отправитель и приёмник!! GBUS_FULL
GBus<RX_PIN, GBUS_FULL> rx(RX_ADDR, 15);


// Если значенике первого байт больше нуля - запускается программа.
// счётчик вычитает значени. Если значения дойдт до "0" программа останавливается с проигрышем
// если приходит по ИК коду верное значение, программа останавливается с выигришем. передаётся второй байт "1" и обнуляется первый байт


#include <GyverStepper.h>
GStepper<STEPPER4WIRE> stepper(2048, 10, 11, 12, 13);
// мотор с драйвером ULN2003 подключается по порядку пинов, но крайние нужно поменять местами
// то есть у меня подключено D2-IN1, D3-IN2, D4-IN3, D5-IN4, но в программе поменял 5 и 2

void setup() {
  //Проверим парковку - калибровку
  pinMode(A0, INPUT_PULLUP);  // кнопка на D12 и GND
  stepper.autoPower(true);
  stepper.setMaxSpeed(400);
  stepper.setRunMode(KEEP_SPEED);
  stepper.setSpeedDeg(-80);   // медленно крутимся НАЗАД

  // пока кнопка не нажата
  while (digitalRead(A0)) {
    stepper.tick();
  }

  // вот тут кнопка нажата, сразу вырубаем мотор.
  // Текущее положение также сбрасывается в 0

  // дальше например врубаем FOLLOW_POS
irrecv.enableIRIn(); // Start the receiver

  stepper.setRunMode(FOLLOW_POS);
  stepper.setAcceleration(500);
  stepper.setMaxSpeed(750);    // в шагах/сек
  stepper.reset();
  stepper.brake();
}

void loop() {
  // для начала ждём флаг от I2C.  флаг представляет 1 битное число, сколько секунд  мы ждём ИК сигнал от палочки ( из числа генерируем шим для поднятия вверх)!
  // включаем счётчик отсчёта!
  // ожидаем ИК сигнала от палочки. По приходу правильного сигнала выключаем звук и паркуем мандрагору в низ до pindown. -> Отправляем на мастер флаг что атака закончена.
  // поднимаемся вверх до срабатывания pinup и параллельно машем руками, и включаем звук на плеере ( жарим на всю)
  // Если/Когда поднялись - шевелим руками и по чуть чуть опускаемся вниз и опять поднимаемся вверх
  // Если по окончанию таймера не пришёл ИК код - паркуем мандрагору до pindown выключаем звук отправляем на мастер что атака не произведена

  if (!stepper.tick()) {
   // flag = 1;
  }


  rx.tick();

  if (rx.gotData()) {
    rx.readData(rec);
    counter = rec * 10; // получаем счётчик на 1/10 секунды
    tmrdown = rec;
    targetdata=0;

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
        counter = 0;

        tmrdown = 99;
      }

    }
  }

if (counter==0)
{stepper.setTarget(0);}

  if (counter > 0)
  {
    if (flag == 1)
    {
      stepper.setTarget(targetdata);
      flag = 0;
    }
  }




  if (millis() - tmr2 >= 100) // каждые 1/10 сек
  {
    tmr2 = millis();
    if  (counter > 0) {
      counter--;
    }

  }



  if (millis() - tmr >= 1000) // каждые 1/10 сек
  {
    flag=1;
    targetdata<=4500 ? targetdata=targetdata+550:targetdata=targetdata ;
    tmr = millis();
    if  (tmrdown > 0)
    {
      tmrdown--;
      
    }

  }

}
