
#include <ServoSmooth.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <DFPlayerMini_Fast.h>
SoftwareSerial mySerial(12, 13); // RX, TX
DFPlayerMini_Fast myMP3;
#include <IRremote.h>

int RECV_PIN = A2;
#include <avr/io.h>
#include <avr/interrupt.h>

#define RIGHT_HAND_PIN    5 // 110 раскрыто 0 закрыто
#define LEFT_HAND_PIN    6 // 170 раскрыто 60 закрыто
#define HEAD_PIN       9 // 0 - вверх 90 в низ

IRrecv irrecv(RECV_PIN);
decode_results results;


ServoSmooth rightHand;
ServoSmooth leftHand;
ServoSmooth head;

int pos;
int cloc;
int servoSpeed;
int a,b,c;


int nano2=0;
byte high[2];

unsigned long period_time = 50;
// переменная таймера, максимально большой целочисленный тип (он же uint32_t)
unsigned long my_timer;
unsigned long irvalue;

void setup() {
  
  Serial.begin(115200);
  mySerial.begin(9600);
  myMP3.begin(mySerial);

  Serial.println(F("Startup"));
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Setting volume to max");
  myMP3.volume(25);
  delay(20);
  
rightHand.attach(RIGHT_HAND_PIN,600, 2400,80);  // 600 и 2400 - длины импульсов, при которых
leftHand.attach(LEFT_HAND_PIN,600, 2400,80);  // 600 и 2400 - длины импульсов, при которых
head.attach(HEAD_PIN,600, 2400,90);  // 600 и 2400 - длины импульсов, при которых


rightHand.setSpeed(servoSpeed);   // ограничить скорость
rightHand.setAccel(0.2);  // установить ускорение (разгон и торможение)
leftHand.setSpeed(servoSpeed);   // ограничить скорость
leftHand.setAccel(0.2);  // установить ускорение (разгон и торможение)


  Wire.begin(2);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event

  
}


void loop() {
    // Значит как. по i2c получаем комманду стартавать ведьму. комманда стартует таймер (число секунд активности приходит по i2c  и из него строится скорость движений.  и звук голоса ведьмы
    // Если в это время мимо таймера приходит ИК сигнал "атака" - 
    // включается голос  проигравшей ведьмы
    // Включается сброс движения до "засыпания" 
    // Отправка сигнала по i2c  что произведена атака
    // Если нет 
    // вклбчается звук победы
    // фиксируется движение на "раскрытии"
    // отправка по i2c  что проиграла
servoSpeed=150;
rightHand.setSpeed(servoSpeed);   // ограничить скорость
rightHand.setAccel(0.1);  // установить ускорение (разгон и торможение)
leftHand.setSpeed(servoSpeed);   // ограничить скорость
leftHand.setAccel(0.1);  // установить ускорение (разгон и торможение)
head.setSpeed(servoSpeed);   // ограничить скорость
head.setAccel(0.1);  // установить ускорение (разгон и торможение)

  // желаемая позиция задаётся методом setTarget (импульс) или setTargetDeg (угол), далее
  // при вызове tick() производится автоматическое движение сервы
  // с заданным ускорением и ограничением скорости
  // servo.tick();   // здесь происходит движение серво по встроенному таймеру!
 
rightHand.tick();
delay(50);
leftHand.tick();
delay(50);
head.tick();
delay(50);


if (irrecv.decode(&results)) 
  {
  //  Serial.println(results.value);
    irvalue = results.value;
    irrecv.resume(); // Receive the next value
    Serial.println(irvalue);
   if ((irvalue == 1111000003) || (irvalue == 16726215)) // атака проведена верно  - сворачиваемся.
  {
 
//Закрыть
rightHand.setTargetDeg(50);               // и отправляем на серво
leftHand.setTargetDeg(50);               // и отправляем на серво
head.setTargetDeg(80);               // и отправляем на серво
nano2=nano2+10;
 myMP3.play(4);
 Serial.println(results.value);
    }
  if ((irvalue == 1111000003)||(irvalue == 1100000003))
  {

rightHand.setTargetDeg(0);               // и отправляем на серво
leftHand.setTargetDeg(170);               // и отправляем на серво
head.setTargetDeg(0);               // и отправляем на серво
nano2=nano2-10;
myMP3.play(3);
Serial.println(results.value);
  }
    
     
  }  
   
 
}  

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  high[0] = (nano2 >> 8);
  high[1] = (nano2 & 0xff);
  Wire.write(high[0]); // respond with message of 2 bytes
  Wire.write(high[1]);
}

    
