
#define DECODE_DISTANCE_WIDTH  // Universal decoder for pulse distance width protocols
//#define DECODE_HASH

#include <IRremote.h>

long heartbeatMillis, flash_milis;
long time;
int on=0;


void setup() {

  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
// Routines to set and clear bits (used in the sleep code)
  IrReceiver.begin(PB4);
           // Delay for 1 second
  pinMode(PB1, OUTPUT);  
  digitalWrite(PB1, 0);
}

void loop() {

 
if (on==0)
{
    digitalWrite(PB1, 0); 
}
else {

    if (millis() - flash_milis>= 100)
    {
      flash_milis = millis();
    //  digitalWrite(PB1, 1); 
  analogWrite(PB1, random(111,228));  
    }
}




  //is it time to toggle the heartbeatLED ?
    if ((millis() - heartbeatMillis >= 95000)&&(on==1))
    {
      //restart this TIMER
      heartbeatMillis = millis();
         on=0;   
      //Toggle the heartbeatLED

    }


  if (IrReceiver.decode()) {
    unsigned long irValue = IrReceiver.decodedIRData.decodedRawData;  // Получение значения ИК сигнала

    // New LSB first 32-bit IR data code
    uint32_t newCode = 0;

    for (int i = 0; i < 32; i++) {
      // Extract the ith bit from the old code
      uint32_t bit = (irValue >> (31 - i)) & 1;

      // Set the ith bit in the new code
      newCode |= (bit << i);
    }
    if ((newCode == 1111000001) || (newCode == 16724175)) {
      heartbeatMillis = millis();
      flash_milis = millis();
    on=1;
        }

        if ((newCode == 1111000002) || (newCode == 16718055)) {


      on=0; 

    }
    IrReceiver.resume();
    newCode = 0;
  }

}
