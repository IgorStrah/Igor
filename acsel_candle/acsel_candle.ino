
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
MPU6050 accelgyro;
int moveif=-50, cloc;
int16_t ax, ay, az;
int16_t gx, gy, gz;
volatile bool f = 0;
int myAxel[15][3];
// переменные для хранения значений

unsigned int x, y, z;


#define LED_PIN 13
bool blinkState = false;

void setup() {
    Wire.begin();
 
    accelgyro.initialize();
    
}

void loop() {

  wdt_enable(WDTO_120MS); //Задаем интервал сторожевого таймера (2с)
  WDTCSR |= (1 << WDIE); //Устанавливаем бит WDIE регистра WDTCSR для разрешения прерываний от сторожевого таймера
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Устанавливаем интересующий нас режим
  sleep_mode(); // Переводим МК в спящий режим

accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

for (int i=1; i<15; i++)
{
myAxel[i-1][0]=myAxel[i][0];
myAxel[i-1][1]=myAxel[i][1];
myAxel[i-1][2]=myAxel[i][2];

}

myAxel[14][0]=ax/50;
myAxel[14][1]=ay/50;
myAxel[14][2]=az/50;

if (abs(myAxel[5][0]-myAxel[14][0])>30)
{
 moveif++;


}
if (abs(myAxel[5][1]-myAxel[14][1])>30)
{
 moveif++;


}
if (abs(myAxel[5][2]-myAxel[14][2])>30)
{
  moveif++;

}

   


if (moveif>5)
{

  while (cloc<600)
  {
 analogWrite(5, random(0,50) );
 cloc++;
 delay(20);
  }
  moveif=0;
  cloc=0;
   analogWrite(5, 0) ;
}

       
    
}

ISR (WDT_vect) {
  wdt_disable();
  f = !f;
}
