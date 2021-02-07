#include <Wire.h>                                     // подключаем библиотеку для работы с шиной I2C
#include <iarduino_I2C_connect.h>                     // подключаем библиотеку для соединения arduino по шине I2C
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
DFPlayerMini_Fast myMP3;
SoftwareSerial mySerial(A2, A3); // RX, TX
#include <IRremote.h>
int RECV_PIN = 8;
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



  //  Read date IRDA
  if (irrecv.decode(&results))   // Модуль ИК кода
  {
    irvalue = results.value;
    irrecv.resume(); // Receive the next value
    if (irvalue == 1111000002) // атака проведена верно  - сворачиваемся.
    {
      
      REG_Massive[0] = 0;
      REG_Massive[1] = 9; 
      myMP3.volume(5);
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
      analogWrite(analogOutPin, 0);
      stepcounter=0;
      tmrup=0;
      delay(1000);
     }
  }
REG_Massive[1] = 1; 
}




  
