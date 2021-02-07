#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
DFPlayerMini_Fast myMP3;
SoftwareSerial mySerial(A2, A3); // RX, TX
#include <IRremote.h>
int RECV_PIN = 8;
int counter = 0;
unsigned long irvalue;
uint32_t tmr;
uint32_t tmr2;
uint32_t tmrup;
uint32_t tmrdown;
int stepcounter= 1;
boolean flag;
int start1 = 0;
boolean start2 = 0;
boolean stop1 = 0;
int servostep = 0;
int analogOutPin = 9;
int rec;
#include "GyverBus.h"

#define RX_PIN 2    // пин
#define RX_ADDR 2   // наш адрес

// работает как отправитель и приёмник!! GBUS_FULL
GBus<RX_PIN, GBUS_FULL> rx(RX_ADDR, 15);

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {

 
  irrecv.enableIRIn(); // Start the receiver


  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(25);
  delay(20);
stepcounter=1;
}

void loop() {


  // для начала ждём флаг от .  флаг представляет 1 битное число, сколько секунд  мы ждём ИК сигнал от палочки ( из числа генерируем шим для поднятия вверх)!
  // включаем счётчик отсчёта!
  // ожидаем ИК сигнала от палочки. По приходу правильного сигнала выключаем звук и паркуем мандрагору в низ до pindown. -> Отправляем на мастер флаг что атака закончена.
  // поднимаемся вверх до срабатывания pinup и параллельно машем руками, и включаем звук на плеере ( жарим на всю)
  // Если/Когда поднялись - шевелим руками и по чуть чуть опускаемся вниз и опять поднимаемся вверх
  // Если по окончанию таймера не пришёл ИК код - паркуем мандрагору до pindown выключаем звук отправляем на мастер что атака не произведена

  
rx.tick();
  
  if (rx.gotData()) {
    rx.readData(rec);
    counter = rec*10 ; // получаем счётчик на 1/10 секунды
    //   - разово включаем звук. надо сделать его на подольше.
    myMP3.play(1);
    myMP3.volume(25);
    start2 = 1; 
    tmrup=0;
    tmrdown=rec;
    stepcounter=1;
  digitalWrite(LED_BUILTIN, HIGH);
  }
 
   // приняли запрос request
  if (rx.gotRequest()) {
      // отправили данные
    rx.sendData(rx.getTXaddress(), tmrdown);  // отправить дату
    }

   
if (counter>2)
{
if ((stepcounter==1)&&(tmrup<29))
{
  analogWrite(analogOutPin, 210);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
}
else if ((stepcounter==1)&&(tmrup>29))
{
 stepcounter=2;tmrup=0;
}
if  ((stepcounter==2)&&(tmrup<15))
{
   digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  analogWrite(analogOutPin, 55);
}
else if ((stepcounter==2)&&(tmrup>15))
{
 stepcounter=3;tmrup=0;
 
}
if ((stepcounter==3)&&(tmrup<20))
{
  analogWrite(analogOutPin, 170);
  digitalWrite(LED_BUILTIN, HIGH);  
}
else if ((stepcounter==3)&&(tmrup>20))
{
 stepcounter=2;tmrup=0;
}   


  //  Read date IRDA
  if (irrecv.decode(&results))   // Модуль ИК кода
  {
    irvalue = results.value;
    irrecv.resume(); // Receive the next value
   if ((irvalue == 1111000003)||(irvalue == 16726215)) // атака проведена верно  - сворачиваемся.
    {
      
 
      myMP3.volume(5);
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
      analogWrite(analogOutPin, 0);
      stepcounter=0;
      tmrup=0;
      tmrdown=99;
     }
  }

}

else
{
   digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  analogWrite(analogOutPin, 0);
   stepcounter=0;
   tmrup=0;
  }

  if (millis() - tmr2 >= 1000) // каждые 1/10 сек
  {
   tmr2 = millis();
   if  (counter>1) 
   tmrdown--;
  }

  if (millis() - tmr >= 100) // каждые 1/10 сек
  {
   tmr = millis();
   if  (counter>1) counter--;
   tmrup++;

    


   
  }
  
}
