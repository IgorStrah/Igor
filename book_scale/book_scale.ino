
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150   // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600   // This is the 'maximum' pulse length count (out of 4096)
#define USMIN 1000     // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX 1600     // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz updates
int eyelidup, eyeliddown, eyel,eyer,eyeu,eyed;
  // our servo # counter
  uint8_t servonum = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("8 channel Servo test!");

  pwm.begin();

  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);
nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  Serial.println("Waiting for an ISO14443A Card ...");

  for (uint16_t microsec = 1700; microsec > 1000; microsec--) {
    pwm.writeMicroseconds(4, microsec);
  }
}

// You can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;

  pulselength = 1000000;      // 1,000,000 us per second
  pulselength /= SERVO_FREQ;  // Analog servos run at ~60 Hz updates
  Serial.print(pulselength);
  Serial.println(" us per period");
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength);
  Serial.println(" us per bit");
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

void loop() {

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

  // up eyse
  
  for (uint16_t microsec = 1000; microsec < 1700; microsec++) {
    pwm.writeMicroseconds(4, microsec);
  }

  eyelidup=1695; eyeliddown=1265;
  
   for (uint16_t microsec = 0; microsec < 40; microsec++) {
     eyelidup=eyelidup-3; eyeliddown=eyeliddown+3;
    pwm.writeMicroseconds(3, eyelidup);
    pwm.writeMicroseconds(2, eyeliddown);
  }
delay(555);

   for (uint16_t microsec = 0; microsec < 35; microsec++) {
    eyelidup=eyelidup-3; eyeliddown=eyeliddown+3;
    pwm.writeMicroseconds(3, eyelidup);
    pwm.writeMicroseconds(2, eyeliddown);
  }


 delay(222);
  for (uint16_t microsec = 1000; microsec < 1200; microsec++) {
    pwm.writeMicroseconds(1, microsec);
  }
 delay(222);
   for (uint16_t microsec = 1200; microsec > 1000; microsec--) {
    pwm.writeMicroseconds(1, microsec);
  }

  for (uint16_t microsec = 1000; microsec < 1050; microsec++) {
    pwm.writeMicroseconds(1, microsec);
  }

delay(100);

   for (uint16_t microsec = 60; microsec > 1; microsec--) {
  eyelidup=eyelidup+3; eyeliddown=eyeliddown-3;
    pwm.writeMicroseconds(3, eyelidup);
    pwm.writeMicroseconds(2, eyeliddown);
  }




 delay(555);
  for (uint16_t microsec = 850; microsec < 1450; microsec++) {
    microsec++;  //microsec++;
    pwm.writeMicroseconds(0, microsec);
  }
 delay(555);
   for (uint16_t microsec = 1450; microsec > 850; microsec--) {
    microsec--;   // microsec--;
    pwm.writeMicroseconds(0, microsec);
  }
  for (uint16_t microsec = 850; microsec < 1200; microsec++) {
    microsec++; // microsec++;
    pwm.writeMicroseconds(0, microsec);
  }


   for (uint16_t microsec = 20; microsec > 1; microsec--) {
  eyelidup=eyelidup+3; eyeliddown=eyeliddown-3;
    pwm.writeMicroseconds(3, eyelidup);
    pwm.writeMicroseconds(2, eyeliddown);
  }

 delay(555);
     for (uint16_t microsec = 5; microsec > 1; microsec--) {
  eyelidup=eyelidup+3; eyeliddown=eyeliddown-3;
    pwm.writeMicroseconds(3, eyelidup);
    pwm.writeMicroseconds(2, eyeliddown);
  }


  delay(3000);
  for (uint16_t microsec = 1700; microsec > 1000; microsec--) {
    pwm.writeMicroseconds(4, microsec);
  }


Serial.println(eyelidup);
Serial.println(eyeliddown);



  /*
 delay(1000);
  for (uint16_t microsec = 1050; microsec < 1400; microsec++) {
    pwm.writeMicroseconds(3, microsec);
  }
 delay(1000);
   for (uint16_t microsec = 1400; microsec  1050; microsec--) {
    pwm.writeMicroseconds(3, microsec);
  }
 delay(1000);
  for (uint16_t microsec = 1150; microsec < 1500; microsec++) {
    pwm.writeMicroseconds(2, microsec);
  }
 delay(1000);
   for (uint16_t microsec = 1500; microsec > 1400; microsec--) {
    pwm.writeMicroseconds(2, microsec);
  }





 delay(1000);
  for (uint16_t microsec = 1000; microsec < 1600; microsec++) {
    pwm.writeMicroseconds(1, microsec);
  }
 delay(1000);
   for (uint16_t microsec = 1600; microsec > 1000; microsec--) {
    pwm.writeMicroseconds(1, microsec);
  }

*/





  /*
  for (uint16_t microsec = USMIN; microsec < USMAX; microsec++) {
    pwm.writeMicroseconds(servonum, microsec);
  }

  delay(500);
  for (uint16_t microsec = USMAX; microsec > USMIN; microsec--) {
    pwm.writeMicroseconds(servonum, microsec);
  }



  servonum++;
  if (servonum > 7) servonum = 0; // Testing the first 8 servo channels
  */
}}
