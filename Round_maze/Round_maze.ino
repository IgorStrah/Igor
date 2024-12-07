#include <Arduino.h>
// select only NEC and the universal decoder for pulse distance protocols
#define DECODE_NEC             // Includes Apple and Onkyo
#define DECODE_DISTANCE_WIDTH  // In case NEC is not received correctly. Universal decoder for pulse distance width protocols
#include <IRremote.hpp>
#define DELAY_AFTER_SEND 2000
#define DELAY_AFTER_LOOP 5000
#include <GyverStepper.h>
//GStepper<STEPPER4WIRE> stepper(2048, 5, 3, 4, 2);
GStepper<STEPPER2WIRE> stepper(2048, 5, 2);
uint8_t startgame, cloc, num;
int direc;
uint32_t newCode = 0, code = 0, sendcode;
String ircod;
String updws, lrs;
int  updw, lr, updwold, lrold, updwmap, lrmap, x, y, xr, xl, yu, yd; // ох. не завидую я тебе когда ты будешь пытаться это понять.
void setup() {
  Serial.begin(115200);
  stepper.setRunMode(KEEP_SPEED);  // режим поддержания скорости
  stepper.setSpeedDeg(311);        // в градусах/сек
  stepper.setAcceleration(1500);
  IrReceiver.begin(10);
  IrSender.begin(3);  // Start with IR_SEND_PIN -which is defined in
}

void loop() {
  stepper.tick();

  if (IrReceiver.decode()) {

    code = IrReceiver.decodedIRData.decodedRawData;
    for (int i = 0; i < 32; i++) {
      // Extract the ith bit from the old code
      unsigned long bit = (code >> (31 - i)) & 1;
      // Set the ith bit in the new code
      newCode |= (bit << i);
    }
    Serial.println(newCode);
    IrReceiver.resume();
     ircod = newCode;

    if ((newCode == 1111000004) || (newCode == 16726215)) {
      startgame = 1;
      Serial.print(startgame);
      Serial.print(" startgame ");
    }




    if (newCode != 0) {
      if ((newCode == 1111000001) || (newCode == 16720605)) {
        direc -= 100;
      }
      if ((newCode == 1111000002) || (newCode == 16761405)) {
        direc += 100;
      }
      if ((newCode == 1111000002) || (newCode == 16712445)) {
        direc = 0;
      }

      Serial.println(direc);
      newCode = 0;
    }
  }


  if (startgame==1)
  {

   updws = ircod.substring(0, 2);
   lrs = ircod.substring(3, 5);
   updw = updws.toInt();
   lr = lrs.toInt();

   updw  = map(updw, 20, 90, 900, -900);
   lr =map(lr, 20, 90, -500, +500);
  direc=updw;
     Serial.print(updw);
     Serial.print(" ");
     Serial.print(lr);
     Serial.println("    :     ");

    
  }
  static uint32_t tmr2;
  if (millis() - tmr2 > 150) {
    tmr2 = millis();
    stepper.setSpeed(direc);
   // sendcode = 3033003;
  //  IrSender.sendSAMSUNG(sendcode, 32);
  }
}
