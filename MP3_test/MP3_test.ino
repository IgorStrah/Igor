#include <DFPlayerMini_Fast.h>

#if !defined(UBRR1H)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(A2, A3); // RX, TX
#endif
int i=0;
DFPlayerMini_Fast myMP3;

void setup()
{
  Serial.begin(115200);

#if !defined(UBRR1H)
  mySerial.begin(9600);
  myMP3.begin(mySerial, true);
#else
  Serial1.begin(9600);
  myMP3.begin(Serial1, true);
#endif
  
  Serial.println("Setting volume to max");
  myMP3.volume(30);
  
}

void loop()
{
  myMP3.play(8);
delay(1110);
myMP3.play(10);
  Serial.println(i);
  delay(110010);
  if (i>9)
  {i=0;}
  //do nothing
}
/*
1 - 
2- all
3- spirit
4 - wota
5 - Fire
6 - earth
7 - air
8 ---- cryppi find
9 - shopot
10 - alarm
11 - jcokc
12 - Air

*/


