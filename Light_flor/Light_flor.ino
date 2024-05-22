
#include <IRremote.hpp>

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;
unsigned long code, timing_welcome;
unsigned long newCode;
uint32_t timer = 0;
int step = 0, position, positionold,lightUV;
bool steptic, redy, light;




void setup() {
  // наша задача - при запуске крутить мотор в сторону до нажатия на кнопку
  IrReceiver.begin(3);
  Serial.begin(115200);

  Serial.println("start");
  redy = 0;

  for (int i = 4; i < 12; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
    delay(10);
  }
}
void loop() {

  if (millis() - currentMillis >= 3000)  //test whether the period has elapsed
  {
    redy = 0;
    currentMillis = millis();  //IMPORTANT to save the start time of the current LED state.
    if (lightUV>0) {lightUV--;}
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

  if ((newCode == 1111000001) || (newCode == 16724175)) {

    if (redy == 0) {
      redy = 1;


      if (light == 0) {
        digitalWrite(8, HIGH);
        delay(300);
        digitalWrite(7, HIGH);
        digitalWrite(9, HIGH);
        delay(300);
        digitalWrite(6, HIGH);
        digitalWrite(10, HIGH);
        newCode = 0;
        light=1;
          currentMillis = millis(); 
      } else {
        digitalWrite(8, LOW);
        delay(200);
        digitalWrite(7, LOW);
        digitalWrite(9, LOW);
        delay(200);
        digitalWrite(6, LOW);
        digitalWrite(10, LOW);
        newCode = 0;
        light=0;
          currentMillis = millis(); 
      }
    }
    newCode = 0;
  }


  if ((newCode == 1111000006) || (newCode == 16734885)) {


    for (int i = 4; i < 12; i++) {
      digitalWrite(i, LOW);
      delay(5);
    }

    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(5, LOW);
    delay(1000);
 if (light == 1) {
        digitalWrite(8, HIGH);
        delay(300);
        digitalWrite(7, HIGH);
        digitalWrite(9, HIGH);
        delay(300);
        digitalWrite(6, HIGH);
        digitalWrite(10, HIGH);
        newCode = 0;
        light=1;
          currentMillis = millis(); 
      }
    newCode = 0;
  }


  if ((newCode == 1111000003) || (newCode == 16743045)) {

    for (int i = 4; i < 12; i++) {
      digitalWrite(i, LOW);
      delay(5);
    }
    lightUV=11;

    newCode = 0;
  }
if (lightUV>1)
{
  digitalWrite(4, HIGH);

}
else 
{
    digitalWrite(4, LOW);
    delay(5);
}
}
