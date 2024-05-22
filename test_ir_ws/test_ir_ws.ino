#include <SoftwareSerial.h>
SoftwareSerial mySerial(4,3);  //rx, tx
#include <Adafruit_NeoPixel.h>

#define LED_COUNT 27   // Количество светодиодов в ленте
#define GROUP_SIZE 4   // Размер группы светодиодов

bool fireEffectActive = false;
uint8_t situation = 0;
uint8_t START = 0;
uint8_t x = 0;
uint8_t BIT = 0;
uint8_t Id = 0;
uint8_t Id_inv = 0;
uint8_t Data = 0;
uint8_t Data_back = 0;
uint8_t Data_inv = 0;
uint8_t Repeat = 0;
uint8_t sended = 0;

uint16_t Time_old = 0;
uint16_t Time = 0;
uint16_t TimeDelta = 0;
uint16_t Timer_Fire;
 Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, 0, NEO_GRB + NEO_KHZ800);

void setup(void) {
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  attachInterrupt(0, IR_Read, FALLING);
  pinMode(1,OUTPUT);
  digitalWrite(1,LOW);

    mySerial.begin(9600);
    mySerial.println("hello world");

  strip.setBrightness(255);
  strip.begin();
  strip.show();  // Инициализация ленты
    strip.clear();  // Инициализация ленты

}
 
void loop(void) {
if (Timer_Fire==1)
{
  for (int i=255; i>0; i--) {
 strip.setBrightness(i);
 strip.show();  
 delay(10);
 
}
Timer_Fire=0;
fireEffectActive = false;
}
if (fireEffectActive) {
    fireEffect();
  }


  if(sended == 1) {
    noInterrupts();
    mySerial.println(" ir code ");
     mySerial.println(Data_back);
     delay(100);
    interrupts();

    if (Data_back == 48) { // Замените на код, который соответствует кнопке на вашем ИК-пульте
    strip.setBrightness(255);
    fireEffectActive = !fireEffectActive; 
    Timer_Fire=9000;   // Включаем/выключаем эффект огня
  }
if (Data_back == 24) { // Замените на код, который соответствует кнопке на вашем ИК-пульте
    fireEffectActive = false;
    strip.clear(); 
      strip.show();
  }


  //  DigiUSB.println(Data_back, DEC);
    sended = 0;
  } else {
  delay(10);
  }
}

void IR_Read(void) {
  digitalWrite(1,HIGH);
  Time = micros();
  if (Time_old != 0) {
    TimeDelta = Time - Time_old;
    if ((TimeDelta > 12000)&&(TimeDelta < 14000)) {
      START = 1;
      x = 0;
      situation = 1;
      Id = 0;
      Id_inv = 0;
      Data = 0;
      Data_inv = 0;
    } else if ((TimeDelta > 10000)&&(TimeDelta < 12000)) {
      situation = 2; // repeat
    } else if ((TimeDelta > 1500)&&(TimeDelta < 2500)) {
      situation = 3; //"1"
      BIT = 1;
    } else if ((TimeDelta > 1000)&&(TimeDelta < 1500)) {
      situation = 3; //"0"
      BIT = 0;
    } else situation = 5; 
    if (situation == 3) {
      if (x < 8) {
        Id |= BIT;
        if (x < 7) Id <<= 1;  
        x++;
      } else if (x < 16) {
        Id_inv |= BIT;
        if (x < 15) Id_inv <<= 1;
        x++;
      } else if (x < 24) {
        Data |= BIT;
        if (x < 23) Data <<= 1;
        x++;
      } else if (x < 32) {
        Data_inv |= BIT;
        if (x < 31) {
          Data_inv <<= 1;
        } else {
          
          /* DO SOMETHING HERE */
          sended = 1;
          
          Data_back = Data;
          Repeat = 0;
        }
        x++;
      }
    } else if (situation == 2) {
      if(Repeat == 1) {
        
        /* DO SOMETHING HERE */
        /*sended = 1;*/
        
      } else {
        Repeat = 1;
      }
    }
  }
  Time_old = Time;
  digitalWrite(1,LOW);
}


void fireEffect() {
  // Реализация эффекта колыхания огня на адресной ленте с разными оттенками для групп по 4 пикселя
  for (int i = 0; i < LED_COUNT; i += GROUP_SIZE) {
    int baseFlicker = random(150, 255);  // Основной оттенок для группы
    for (int j = 0; j < GROUP_SIZE; j++) {
      int flicker = baseFlicker - j * 10;  // Различные оттенки внутри группы
      strip.setPixelColor(i + j, strip.Color(flicker, flicker / 2, 0));
    }
  }
  strip.show();
  delay(random(20, 100));  // Задержка между итерациями эффекта
  Timer_Fire--;
}
