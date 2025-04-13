#define TRIG_PIN 0 // PB1
#define ECHO_PIN 2 // PB2
#define OUT_PIN  1 // PB1 (поднимем + здесь)
#define IN_PIN  3 //

const int thresholdDistance = 70; // в см
const int checkCount = 25;         // кол-во проверок подряд
const int delayBetweenChecks = 200; // мс
int count = 0;
void setup() {

  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(OUT_PIN, OUTPUT);
  digitalWrite(OUT_PIN, LOW);
    pinMode(IN_PIN, INPUT_PULLUP);
        digitalWrite(OUT_PIN, HIGH);
    delay(500); // держим HIGH 1 сек
    digitalWrite(OUT_PIN, LOW);
  
}




void loop() {

  long duration, distance;

  if (digitalRead(IN_PIN)==0)
  {

 digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  distance = duration * 0.034 / 2;

  
    if (distance < thresholdDistance) {
 
      count++;
    } else {
      // если хотя бы один меньше — обнуляем и выходим
      count = 0;
    }

    delay(delayBetweenChecks);
  

  // если все 20 измерений были больше 800 см
  if (count >checkCount) {
    digitalWrite(OUT_PIN, HIGH);
    delay(1000); // держим HIGH 1 сек
    digitalWrite(OUT_PIN, LOW);
        delay(1000); // держим HIGH 1 сек
     digitalWrite(OUT_PIN, HIGH);
    delay(1000); // держим HIGH 1 сек
    digitalWrite(OUT_PIN, LOW);
      delay(60000); // держим HIGH 1 сек
  }



  }

  

}
