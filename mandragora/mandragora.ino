#include <Servo.h>

#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
DFPlayerMini_Fast myMP3;
SoftwareSerial mySerial(A2, A3); // RX, TX
#include <IRremote.h>
int RECV_PIN = 1;
int counter = 0;
unsigned long irvalue;
uint32_t tmr;
boolean flag;
boolean start1 = 0;
boolean start2 = 0;
boolean stop1 = 0;
int servostep = 0;
int servocloc = 0;
boolean servostart = 0;

int muveflag; // 0-stop, 1-up, 2-down.
int engdir = 0; // - driver is move. i wait pin fignal for stop




Servo lefthend;
Servo righthend;
IRrecv irrecv(RECV_PIN);
decode_results results;

#define P1 5
#define P2 4
#define P3 7
#define P4 6
#define pinup A5
#define pindown A4


// Если значенике первого байт больше нуля - запускается программа.
// счётчик вычитает значени. Если значения дойдт до "0" программа останавливается с проигрышем
// если приходит по ИК коду верное значение, программа останавливается с выигришем. передаётся второй байт "1" и обнуляется первый байт



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

  pinMode(pinup, INPUT_PULLUP); // вход с подтяжкой
  pinMode(pindown, INPUT_PULLUP); // вход с подтяжкой
  irrecv.enableIRIn(); // Start the receiver


  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(25);
  delay(20);

  lefthend.attach(10);
  righthend.attach(11);

}

void loop() {


  // для начала ждём флаг от I2C.  флаг представляет 1 битное число, сколько секунд  мы ждём ИК сигнал от палочки ( из числа генерируем шим для поднятия вверх)!
  // включаем счётчик отсчёта!
  // ожидаем ИК сигнала от палочки. По приходу правильного сигнала выключаем звук и паркуем мандрагору в низ до pindown. -> Отправляем на мастер флаг что атака закончена.
  // поднимаемся вверх до срабатывания pinup и параллельно машем руками, и включаем звук на плеере ( жарим на всю)
  // Если/Когда поднялись - шевелим руками и по чуть чуть опускаемся вниз и опять поднимаемся вверх
  // Если по окончанию таймера не пришёл ИК код - паркуем мандрагору до pindown выключаем звук отправляем на мастер что атака не произведена



  //Проверим парковку - калибровку
  if (start1 == 0) {
    if (((digitalRead(pindown) == 1) && (digitalRead(pinup) == 0)) || ((digitalRead(pindown) == 1) && (digitalRead(pinup) == 1)))
      //паркуемся. потом будем думать.
    {
      while ((digitalRead(pindown) != 0) )
      {
        drivermove(2, 120) ; // down
      }

      drivermove(0, 0);
      muveflag = 0; //stop
      engdir = 0; //motor is stoped
      delay(500);
    }
    start1 = 1;
  }



  if ((start2 == 0))// && ) // запуск движения, не цекличное.
  {
    muveflag = 1; // UP!
    counter = 1;
    //   - разово включаем звук. надо сделать его на подольше.
    myMP3.play(1);
    myMP3.volume(25);
    start2 = 1;
    servostart = 1;
    lefthend.attach(10);
    righthend.attach(11);
  }

  if (irrecv.decode(&results))   // Модуль ИК кода
  {
    irvalue = results.value;
    irrecv.resume(); // Receive the next value

    if ((irvalue == 1111000002) || (irvalue == 16726215)) // атака проведена верно  - сворачиваемся.
    {
      muveflag = 2;

      myMP3.volume(5);
      start2 = 0;

    }
  }


  if (counter <= 1) // закончилось время
  { engdir = 0;
    muveflag = 2;

    myMP3.volume(10);
    start2 = 0;
    servostart = 0;


    lefthend.detach();
    righthend.detach();
  }




  if (millis() - tmr >= 100) // каждые 1/10 сек
  {
     tmr = millis();
    counter--;
    servocloc++;
    if (servocloc == 6)
    {
      servocloc = 0;

      if (servostart == 1)
      {
        servostep++;
        if (servostep == 2) {
          servostep = 0;
        }
        if (servostep == 1)
        {

          lefthend.write(25);
          righthend.write(25);
        }
        else
        {

          lefthend.write(155);
          righthend.write(155);

        }

      }


    }



  }






  // дальше процедура вверх и потом вниз. ставим флаг. если лаг = 1 то мы поднимаемся, если 2  - опускаемся.


  if      (((muveflag == 1) && (digitalRead(pinup) == 0) && (engdir == 0)))
  {
    engdir = 1;
    drivermove(1, 160) ; // up

    // delay(100);
  }
  else if (((muveflag == 2) && (digitalRead(pindown) == 1) && (engdir == 0)))
  {
    engdir = 1;
    drivermove(2, 100) ; // down
    //   delay(100);
  }



  // Если двигались вверхз - ждём верхнюю кнопку, если вниз - нижнюю.
  if  (((muveflag == 1) && (digitalRead(pinup) == 1)) || ((muveflag == 2) && (digitalRead(pindown) == 0)))
  {
    engdir = 0;
    drivermove(0, 0) ; // up
    muveflag = 0; //stop

  }


}



int drivermove(byte enginedirection, byte speadmove) {

  //Модуль стоп. Включается всегда перед переключением что-бы не каратнуть

  digitalWrite(P1, 0);
  digitalWrite(P2, 0);
  digitalWrite(P3, 0);
  digitalWrite(P4, 0);
  delay(10);


  if (enginedirection == 0) // STOP
  {
    digitalWrite(P1, 0);
    digitalWrite(P2, 0);
    digitalWrite(P3, 0);
    digitalWrite(P4, 0);
    delay(10);
  }

  else if (enginedirection == 1) // UP
  {
    digitalWrite(P2, 0);    // вырубить Р канальный второго плеча
    digitalWrite(P4, 0);    // вырубить N канальный второго плеча
    delayMicroseconds(5);   // задержечка на переключение на всякий случай
    digitalWrite(P3, 1);    // врубить Р канальный первого плеча
    analogWrite(P1, speadmove);    // ЖАРИТЬ ШИМ!
    delay(100);
  }
  else if (enginedirection == 2) // DOWN
  {
    digitalWrite(P3, 0);    // вырубить Р канальный первого плеча
    digitalWrite(P1, 0);    // вырубить N канальный первого плеча
    delayMicroseconds(5);   // задержечка на переключение на всякий случай
    digitalWrite(P2, 1);    // врубить Р канальный второго плеча
    analogWrite(P4, speadmove);    // ЖАРИТЬ ШИМ!
    delay(100);
  }
}
