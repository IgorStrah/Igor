
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <FastLED.h>
#include <GyverStepper.h>

#define DATA_PIN 3
#define NUM_LEDS 7      
#define PN532_IRQ (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield
GStepper<STEPPER2WIRE> stepper(2048, 4, 5, 6);
CRGB leds[NUM_LEDS];
// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
int NB_NFC_READER = 7;
//byte ssPin[] = {SS_1_PIN, SS_2_PIN};
byte RfidPins[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

void tcaselect(uint8_t i2c_bus) {
  if (i2c_bus > 7) return;
  Wire.beginTransmission(0x70);
  Wire.write(1 << i2c_bus);
  Wire.endTransmission();
}

void setup(void) {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(115200);
  while (!Serial) delay(10);  // for Leonardo/Micro/Zero
  Serial.println("Hello!");
  Wire.begin();
  Wire.beginTransmission(0x70);
  Wire.write(1 << 0);
  Wire.endTransmission();
  for (int i = 0; i < NB_NFC_READER; i++) {
    tcaselect(i);
    nfc.begin();
    leds[i] = CRGB::Red;
    FastLED.show();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
      Serial.print("Didn't find PN53x board");
      while (1)
        ;  // halt
    }
    // Got ok data, print it out!
    Serial.print("Num reader: ");
    Serial.print(i);

    // Got ok data, print it out!
    Serial.print(" Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);

    delay(100);
    leds[i] = CRGB::Black;
    FastLED.show();
  }




}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint32_t cardid;

  for (int i = 0; i < NB_NFC_READER+1; i++) {

    leds[i] = CRGB::Black;
    
    
    tcaselect(i);
    delay(20);
    nfc.begin();
    delay(150);
   
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
  
  if (success) {
    for (byte i2 = 0; i2 < uidLength; i2++) {

      if (i2 == 0) {
        cardid = uid[i2];
        cardid <<= 8;
      } else {
        {
          cardid |= uid[i2];
          cardid <<= 8;
        }
      }
    }

 
    Serial.print(" Reader : ");  //  "Сообщение: "
    Serial.print(i);     //  "Сообщение: "
    Serial.print(" cardid : ");  //  "Сообщение: "
    Serial.println(cardid);      //  "Сообщение: "
    delay(100);
  }
  }
      FastLED.show();
}

/*

  if (millis() - timing2 > 1000) {
     timing2 = millis();
 FastLED.show();
  }
  */
