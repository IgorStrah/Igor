#define STRIP_PIN 3  // пин ленты
#define NUMLEDS 35   // кол-во светодиодов


#include <Adafruit_PCF8574.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <IRremote.hpp>
#define COLOR_DEBTH 3
#include <microLED.h>  // подключаем библу
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2813, ORDER_GRB, CLI_AVER> strip;

#define STRIP_PIN 3  // пин ленты
#define NUMLEDS 8    // кол-во светодиодов
#define PN532_IRQ (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield
byte reader;
int NB_NFC_READER = 7;
unsigned long newCode;
unsigned long code;
unsigned long timing, timing2;
#include <FastLEDsupport.h>
DEFINE_GRADIENT_PALETTE(heatmap_gp){
  // делаем палитру огня
  0, 0, 0, 0,         // black
  128, 255, 0, 0,     // red
  224, 255, 255, 0,   // bright yellow
  255, 255, 255, 255  // full white
};
CRGBPalette16 fire_p = heatmap_gp;

bool is_lantern_on = false;

Adafruit_PCF8574 pcf;

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
byte RfidPins[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

unsigned long Uidtable[8];

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");
  delay(20);
  strip.setBrightness(160);
  Wire.begin();
  Wire.beginTransmission(0x70);
  Wire.write(1 << 0);
  Wire.endTransmission();
  for (int i = 0; i < NB_NFC_READER; i++) {
    tcaselect(i);
    Serial.print(" read: ");
    Serial.print(i);
    Serial.print(" NB_NFC_READER: ");
    Serial.print(NB_NFC_READER);
    nfc.begin();
    strip.clear();
    strip.leds[i] = mRGB(0, 230, 60);
    strip.show();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
      Serial.print("Didn't find PN53x board");
      while (1)
        ;  // halt
    }
    // Got ok data, print it out!
    Serial.print(" Num reader: ");
    Serial.print(i);

    // Got ok data, print it out!
    Serial.print(" Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);

    delay(100);
    strip.clear();
    strip.show();
  }
  IrReceiver.begin(2);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  if (!pcf.begin(0x20, &Wire)) {
    Serial.println("Couldn't find PCF8574");
    //  while (1);
  }
  for (uint8_t p = 0; p < 5; p++) {
    pcf.pinMode(p, OUTPUT);
    delay(1000);
  }
}


void loop(void) {
  if (millis() - timing > 50) {
    timing = millis();
    reader == 7 ? reader = 0 : reader++;
    Uidtable[reader] = ReadUid(reader);
    // Serial.print(" cardid : ");        //  "Сообщение: "
    // Serial.println(Uidtable[reader]);  //  "Сообщение: "
    if (is_lantern_on) {
      Serial.println("Latent mode triggered");
      latent();
    }
  }

  if (IrReceiver.decode()) {
    newCode = 0;
    code = 0;
    code = IrReceiver.decodedIRData.decodedRawData;
    for (int i = 0; i < 32; i++) {
      // Extract the ith bit from the old code
      unsigned long bit = (code >> (31 - i)) & 1;
      newCode |= (bit << i);
    }
    Serial.println(newCode);
    delay(100);
    IrReceiver.resume();
  }

  if ((newCode == 1111000001) || (newCode == 16724175)) {
    newCode = 0;
    Serial.println("Lantern triggered");
    is_lantern_on = !is_lantern_on;
  }

  
}



unsigned long ReadUid(byte numReader) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;
  uint32_t cardid = 0;

  tcaselect(numReader);
  delay(10);
  nfc.begin();
  delay(50);

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

    //  Serial.print(" Reader : ");  //  "Сообщение: "
    //  Serial.print(numReader);     //  "Сообщение: "
    //  Serial.print(" cardid : ");  //  "Сообщение: "
    //  Serial.println(cardid);      //  "Сообщение: "
    return (cardid);
  }
  return (cardid);
}

void tcaselect(uint8_t i2c_bus) {
  if (i2c_bus > 7) return;
  Wire.beginTransmission(0x70);
  Wire.write(1 << i2c_bus);
  Wire.endTransmission();
}


void latent() {
  static int count = 0;

  for (int i = 7; i < 34; i++) {
    count += 2;
    strip.set(i, CRGBtoData(ColorFromPalette(fire_p, inoise8(i * 25, count), 255, LINEARBLEND)));
  }
  strip.show();
}
