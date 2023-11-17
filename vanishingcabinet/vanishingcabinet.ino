
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <FastLED.h>
#include <GyverStepper.h>
#include <IRremote.hpp>
  // крутим мотор туда-сюда плавно с ускорением

#define DATA_PIN 3
#define NUM_LEDS 8     
#define PN532_IRQ (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield
GStepper<STEPPER2WIRE> stepper(2048, 4, 5, 6);
GStepper<STEPPER4WIRE> stepper2(2048, 10, 8, 9, 7);

int step_rev;
int NB_NFC_READER = 8;
unsigned long newCode;
unsigned long code;

CRGB leds[NUM_LEDS];

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
byte RfidPins[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

void tcaselect(uint8_t i2c_bus) {
  if (i2c_bus > 7) return;
  Wire.beginTransmission(0x70);
  Wire.write(1 << i2c_bus);
  Wire.endTransmission();
}

void setup(void) {
 //  pinMode(6, OUTPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(115200);
  while (!Serial) delay(10);  // for Leonardo/Micro/Zero
  Serial.println("Hello!");
  Wire.begin();
  Wire.beginTransmission(0x70);
  Wire.write(1 << 0);
  Wire.endTransmission();
  for (int i = 0; i < NB_NFC_READER; i++) {
    Serial.print(" read: ");
    Serial.print(i);
    Serial.print(" NB_NFC_READER: ");
    Serial.print(NB_NFC_READER);
    nfc.begin();
    leds[i] = CRGB::Red;
    FastLED.show();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
      Serial.print("Didn't find PN53x board");
      while (1);  // halt
    }
    // Got ok data, print it out!
    Serial.print(" Num reader: ");
    Serial.print(i);

    // Got ok data, print it out!
    Serial.print(" Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);

    delay(500);
    leds[i] = CRGB::Black;
    FastLED.show();
  IrReceiver.begin(A2);
    pinMode(11, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);
  }

//Stepper_calibrated();

}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)


  // и качаемся в 20 шагах от кнопки и до 300
  static bool dir;
  if (!stepper.tick()) {

  }




  int buttonState = digitalRead(A0);




    if (IrReceiver.decode()) {
      newCode=0;
      code=0;
      code=IrReceiver.decodedIRData.decodedRawData;

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

if  ((newCode == 1111000002)||(newCode == 16718055))
{
}

  for (int i = 0; i < NB_NFC_READER; i++) {

    leds[i] = CRGB::Black;
    
    
    tcaselect(i);
    delay(10);
    nfc.begin();
    delay(50);
   
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
  
    if (success) {
        leds[i] = CRGB::White;
         Serial.print("test ");
         Serial.println(leds[i].getAverageLight());
        

      // Display some basic information about the card
      Serial.print("Reader ");
      Serial.print(i);
      Serial.print("  UID Value: ");
      nfc.PrintHex(uid, uidLength);

      if (uidLength == 4) {
        // We probably have a Mifare Classic card ...
        uint32_t cardid = uid[0];
        cardid <<= 8;
        cardid |= uid[1];
        cardid <<= 8;
        cardid |= uid[2];
        cardid <<= 8;
        cardid |= uid[3];
        Serial.print("Seems to be a Mifare Classic card #");
        Serial.println(cardid);
      }
    }
  }
      FastLED.show();
}

void Stepper_calibrated()
{
  stepper.autoPower(true);
   stepper2.autoPower(true);
  pinMode(12, INPUT_PULLUP);  // кнопка на D12 и GND
  stepper.setRunMode(KEEP_SPEED);
  stepper.setSpeedDeg(-60);   // медленно крутимся НАЗАД
  while(digitalRead(12)) {    
    stepper.tick(); 
  }

  stepper.reset();
  stepper.setSpeedDeg(-20);
  while (stepper.getCurrent() != -390) {
    stepper.tick(); 
      Serial.println(stepper.getCurrent());
   stepper.tick();
  }

  stepper.reset();
  stepper.setRunMode(FOLLOW_POS);
  stepper.stop();


  pinMode(11, INPUT_PULLUP);  // кнопка на D12 и GND
  
  stepper2.setRunMode(KEEP_SPEED);
  stepper2.setSpeedDeg(-70);   // медленно крутимся НАЗАД

  // пока кнопка не нажата
  while(digitalRead(11)) {    
    stepper2.tick();
	// yield();	// для esp8266
  }
 
  stepper2.reset();
  stepper2.setSpeedDeg(60);
  while (stepper2.getCurrent() != 1550) {
   stepper2.tick();
  }

  stepper2.reset();
  stepper2.setSpeedDeg(-30);   // медленно крутимся НАЗАД

  // пока кнопка не нажата
  while(digitalRead(11)) {    
    stepper2.tick();
  }


  stepper2.reset();
  stepper2.setSpeedDeg(33);
  while (stepper2.getCurrent() != 1380) {
   stepper2.tick();
  }
  stepper2.reset();
  delay(3000);
  stepper2.setSpeedDeg(60);
  stepper2.setRunMode(FOLLOW_POS);
}


void Stepper_parking(int steppernum, int pinparking, int parkingstep)
{

  stepper.autoPower(true);
  stepper.setRunMode(KEEP_SPEED);
  stepper.setSpeedDeg(-60);   // медленно крутимся НАЗАД
  while(digitalRead(pinparking)) {    
    stepper.tick(); 
  }
  stepper.reset();
  stepper.setSpeedDeg(-20);
  while (stepper.getCurrent() != -parkingstep) {
  stepper.tick(); 
   Serial.println(stepper.getCurrent());
   stepper.tick();
  }

  stepper.reset();
  stepper.setRunMode(FOLLOW_POS);
  stepper.stop();







  


  stepper2.reset();
  stepper2.setSpeedDeg(60);
  while (stepper2.getCurrent() != 1550) {
   stepper2.tick();
  }

  stepper2.reset();
  stepper2.setSpeedDeg(-30);   // медленно крутимся НАЗАД

  // пока кнопка не нажата
  while(digitalRead(11)) {    
    stepper2.tick();
  }


  stepper2.reset();
  stepper2.setSpeedDeg(33);
  while (stepper2.getCurrent() != 1380) {
   stepper2.tick();
  }
  stepper2.reset();
  delay(3000);
  stepper2.setSpeedDeg(60);
  stepper2.setRunMode(FOLLOW_POS);


}