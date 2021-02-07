#include <Stepper.h>
#include <Wire.h>                                     // подключаем библиотеку для работы с шиной I2C
#include <iarduino_I2C_connect.h>                     // подключаем библиотеку для соединения arduino по шине I2C
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



const int stepsPerRevolution = 200;

IRrecv irrecv(RECV_PIN);
decode_results results;

#define pinup 2
#define pindown 3

iarduino_I2C_connect I2C2;                            // объявляем переменную для работы c библиотекой iarduino_I2C_connect
byte  REG_Massive[3];                  // Три значения. первый байт - сколько секунд работаем, вторйо байт - что приняли тот нужный ИК код ( равез 1 или 0), 3 - скорость с которой работаем.
// Если значенике первого байт больше нуля - запускается программа.
// счётчик вычитает значени. Если значения дойдт до "0" программа останавливается с проигрышем
// если приходит по ИК коду верное значение, программа останавливается с выигришем. передаётся второй байт "1" и обнуляется первый байт

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

boolean switch_flag;
int potent, duty;
int left_min, right_min;

void setup() {

  myStepper.setSpeed(250);

  pinMode(pinup, INPUT_PULLUP); // вход с подтяжкой
  pinMode(pindown, INPUT_PULLUP); // вход с подтяжкой
  irrecv.enableIRIn(); // Start the receiver


  Wire.begin(0x03);                                   // инициируем подключение к шине I2C в качестве ведомого (slave) устройства, с указанием своего адреса на шине.
  I2C2.begin(REG_Massive);

  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(25);
  delay(20);


  Serial.begin(9600);
}

void loop() {


  // для начала ждём флаг от I2C.  флаг представляет 1 битное число, сколько секунд  мы ждём ИК сигнал от палочки ( из числа генерируем шим для поднятия вверх)!
  // включаем счётчик отсчёта!
  // ожидаем ИК сигнала от палочки. По приходу правильного сигнала выключаем звук и паркуем мандрагору в низ до pindown. -> Отправляем на мастер флаг что атака закончена.
  // поднимаемся вверх до срабатывания pinup и параллельно машем руками, и включаем звук на плеере ( жарим на всю)
  // Если/Когда поднялись - шевелим руками и по чуть чуть опускаемся вниз и опять поднимаемся вверх
  // Если по окончанию таймера не пришёл ИК код - паркуем мандрагору до pindown выключаем звук отправляем на мастер что атака не произведена


  // REG_Massive[0]; // байт счётчика
  // REG_Massive[1;] // байт флага верного ИК кода. 1 если верный. ИК на каждый вбивается индивидуально в коде.

  //Проверим парковку - калибровку
  if (start1 == 0) {
    //паркуемся. потом будем думать.
    while  (digitalRead(pinup) == 1)
    {
  //  Serial.println("start1.  0 ") ;
      myStepper.step(100 ); // 1 one turnover
    }
 //     delay(500);
    stopsw();
    start1 = 1;
                   }
  if ((start2 == 0) && (REG_Massive[0] > 1 )) // запуск движения, не цекличное.
  {
    muveflag = 2; // DOWN!
    counter = REG_Massive[0] ; // получаем счётчик на 1/10 секунды
    //   - разово включаем звук. надо сделать его на подольше.
    myMP3.play(1);
    myMP3.volume(25);
    start2 = 1; 
    REG_Massive[1] = 0;
  }

/*
   //  Read date IRDA
  if (irrecv.decode(&results))   // Модуль ИК кода
  {
    irvalue = results.value;
    irrecv.resume(); // Receive the next value

    if (irvalue == 1100000001) // атака проведена верно  - сворачиваемся.
    {
      muveflag = 1; // parking
      REG_Massive[0] = 0;
      REG_Massive[1] = 0;
      myMP3.volume(5);
      start2 = 0;
      start1 = 1;
     }
  }
*/
  if (counter == 1) // закончилось время
  { 
    muveflag = 1; //UP
    REG_Massive[0] = 0;
    REG_Massive[1] = 5;
    myMP3.volume(10);
    start2 = 0;
    stopsw();
    Serial.println(" stop.  1 ") ;
   }
   if ((digitalRead(pinup) == 0) || (digitalRead(pindown) == 0))
{  
      stopsw();  
}
  // дальше процедура вверх и потом вниз. ставим флаг. если лаг = 1 то мы поднимаемся, если 2  - опускаемся.
  if      (((muveflag == 1) && (digitalRead(pinup) == 1) ))
  {   myStepper.step(200 ); // 1 one turnover   
  }
  else if (((muveflag == 2) && (digitalRead(pindown) == 1) ))
  {   myStepper.step(-200 ); // 1 one turnover  
  }




  if (millis() - tmr >= 100) // каждые 1/10 сек
  {
    tmr = millis();
    counter--;
  }
  
}



int stopsw() {
  //Модуль стоп. Включается всегда перед переключением что-бы не каратнуть
  digitalWrite(8, 0);
  digitalWrite(9, 0);
  digitalWrite(10, 0);
  digitalWrite(11, 0);
  delay(10);
}
