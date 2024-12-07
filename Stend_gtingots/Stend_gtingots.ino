
#include <IRremote.hpp>
#include <Servo.h>
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;
unsigned long code, timing_welcome;
unsigned long newCode;
uint32_t timer = 0;
int step = 0, position, positionold;
bool steptic, redy;
Servo myservo1;



void setup() {
  // наша задача - при запуске крутить мотор в сторону до нажатия на кнопку
  IrReceiver.begin(3);
  Serial.begin(115200);

  Serial.println("start");
  redy = 0;
}
void loop() {

  if (millis() - currentMillis >= 5000)  //test whether the period has elapsed
  {
    redy = 0;
    currentMillis = millis();  //IMPORTANT to save the start time of the current LED state.
  }


  if (IrReceiver.decode()) {
    newCode = 0;
    code = 0;
    code = IrReceiver.decodedIRData.decodedRawData;

    for (int i = 0; i < 32; i++) {
      // Extract the ith bit from the old code
      unsigned long bit = (code >> (31 - i)) & 1;

      // Set the ith bit in the new code
      newCode |= (bit << i);
    }
    Serial.println(newCode);

    delay(100);
    //IrReceiver.decodedIRData.decodedRawData();
    IrReceiver.resume();
  }

  if ((newCode == 1111000005) || (newCode == 16726215)) {
                                            
    if (redy == 0) {
        redy = 1;



        myservo1.attach(2);  // Coim
        delay(100);
        myservo1.write(12);
        delay(500);
        myservo1.write(180);
        delay(800);
        myservo1.detach();
        newCode = 0;
        currentMillis = millis();

        newCode = 0;
      }
  }
}
