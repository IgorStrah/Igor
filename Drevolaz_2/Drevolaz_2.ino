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
uint32_t tmrup;
uint32_t tmrdown;
int stepcounter= 1;
boolean flag;
int start1 = 0;
boolean start2 = 0;
boolean stop1 = 0;
int servostep = 0;
int analogOutPin = 9;

IRrecv irrecv(RECV_PIN);
decode_results results;


iarduino_I2C_connect I2C2;                            // объявляем переменную для работы c библиотекой iarduino_I2C_connect
byte  REG_Massive[3];                  // Три значения. первый байт - сколько секунд работаем, вторйо байт - что приняли тот нужный ИК код ( равез 1 или 0), 3 - скорость с которой работаем.
// Если значенике первого байт больше нуля - запускается программа.
// счётчик вычитает значени. Если значения дойдт до "0" программа останавливается с проигрышем
// если приходит по ИК коду верное значение, программа останавливается с выигришем. передаётся второй байт "1" и обнуляется первый байт


boolean switch_flag;
int potent, duty;
int left_min, right_min;

void setup() {


pinMode(LED_BUILTIN, OUTPUT);
 
  irrecv.enableIRIn(); // Start the receiver

  Wire.begin(0x02);     // инициируем подключение к шине I2C в качестве ведомого (slave) устройства, с указанием своего адреса на шине.
  I2C2.begin(REG_Massive);

  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(25);
  delay(20);
stepcounter=1;
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
 
  if (REG_Massive[0] > 1 ) // запуск движения, не цекличное.
  {
    
    counter = REG_Massive[0]*10 ; // получаем счётчик на 1/10 секунды
    //   - разово включаем звук. надо сделать его на подольше.
    myMP3.play(1);
    myMP3.volume(25);
    start2 = 1; 
    REG_Massive[1] = 0;
    REG_Massive[0] = 0;
    tmrup=0;
    stepcounter=1;
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
  //if (counter == 1) // закончилось время
 // { 
 //   REG_Massive[0] = 0;
 //   REG_Massive[1] = 5;
 //   myMP3.volume(10);
 //  }










/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInOutSerial
*/

// These constants won't change. They're used to give names to the pins used:

const int analogOutPin2 = 9; // Analog output pin that the LED is attached to
const int analogOutPin = 10; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)






  // read the analog in value:
  analogWrite(analogOutPin, 170);
  analogWrite(analogOutPin2, 0);

  delay(5500);
  analogWrite(analogOutPin, 90);
  analogWrite(analogOutPin2, 0);

  delay(4000);
  analogWrite(analogOutPin2, 90);
  analogWrite(analogOutPin, 0);
  delay(7500);


//  stop
  analogWrite(analogOutPin2,0);
  analogWrite(analogOutPin, 0);
  delay(200);

  
  analogWrite(analogOutPin, 190);
  analogWrite(analogOutPin2, 0);
  delay(800);

  analogWrite(analogOutPin, 0);
  analogWrite(analogOutPin2, 120);
  delay(600);

  
  analogWrite(analogOutPin, 190);
  analogWrite(analogOutPin2, 0);
  delay(800);

  analogWrite(analogOutPin, 0);
  analogWrite(analogOutPin2, 90);
  delay(500);
  
  analogWrite(analogOutPin, 0);
  analogWrite(analogOutPin2, 120);
  delay(900);























   
if (counter>2)
{
if ((stepcounter==1)&&(tmrup<29))
{
  analogWrite(analogOutPin, 210);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
}
else if ((stepcounter==1)&&(tmrup>29))
{
 stepcounter=2;tmrup=0;tmrdown=0;
 
}



if  ((stepcounter==2)&&(tmrup<15))
{
   digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  analogWrite(analogOutPin, 55);
}
else if ((stepcounter==2)&&(tmrup>15))
{
 stepcounter=3;tmrup=0;tmrdown=0;
 
}


if ((stepcounter==3)&&(tmrup<20))
{
  analogWrite(analogOutPin, 170);
  digitalWrite(LED_BUILTIN, HIGH);  
}
else if ((stepcounter==3)&&(tmrup>20))
{
 stepcounter=2;tmrup=0;tmrdown=0;
}   
}

else
{
   digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  analogWrite(analogOutPin, 0);
   REG_Massive[1] = 5;
   stepcounter=0;
   tmrup=0;
  }



  if (millis() - tmr >= 100) // каждые 1/10 сек
  {
    tmr = millis();
   if  (counter>1) counter--;
   tmrup++;
  }
  
}
