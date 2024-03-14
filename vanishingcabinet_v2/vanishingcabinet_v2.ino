#define EXCLUDE_EXOTIC_PROTOCOLS  // saves around 240 bytes program memory if IrSender.write is used
#define NO_LED_FEEDBACK_CODE
#define DECODE_NEC  // Includes Apple and Onkyo
#define DECODE_SAMSUNG
#define STRIP_PIN 3  // пин ленты
#define NUMLEDS 35   // кол-во светодиодов


#include <Adafruit_PCF8574.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <GyverStepper.h>
#include <IRremote.hpp>
#define COLOR_DEBTH 3
#include <microLED.h>  // подключаем библу
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2813, ORDER_GRB, CLI_AVER> strip;

#define STRIP_PIN 3  // пин ленты
#define NUMLEDS 8    // кол-во светодиодов
#define PN532_IRQ (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield
GStepper<STEPPER2WIRE> stepper(2048, 4, 5, 6);
GStepper<STEPPER4WIRE> stepper2(2048, 10, 8, 9, 7);
byte reader, comparisonuid, movestop;
int step_rev, startstep, positionnow;
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


Adafruit_PCF8574 pcf;

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
byte RfidPins[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
String uidDec, uidDecOld;  // для храниения номера метки в десятичном формате

unsigned long Uidtable[8];

uint32_t potions[][8] = {
  {
    //  0 Философский камень
    902960384,   // 0 безоар
    1498617088,  // 1
    269948160,   // 2
    666047744,   // 3
    594771968,   // 4
    1940264192,  // 5
    2111444224,  // 6
    594771968,   // 7 пятиугольная карточка-рецепт
  },

  {
    //  1 Упокоение пикси
    209392896,   // 0 бокал со змеями
    1687819520,  // 2 флоббер-червь
    1771771136,  // 3 гнездо пикси
    1855526144,  // 6 медальон
    594772224,   // 4 бирюзовая шкатулка
    554308864,   // 5 книга
    1604719872,  // 1 лоскут одеяла
    204150016,   // 7 пятиугольная карточка-рецепт
  },
  {
    //  0 Зелье Плавунчика или Морское зелье +
    0,           // 0 философский камень
    338367744,   // 1 Вытяжка зародыше апаллала
    1498617088,  // 2 Хребты Рыбы-Льва
    666047744,   // 3 Сок мурлакомля
    269948160,   // 4 Стандартный ингридиент Н
    0,           // 5 Морские жёлуди
    0,           // 6 Безумные Многоножки
    0,           // 7 Лёд со дна серебристого озера
  },
  {
    //  0 Зелье Плавунчика или Морское зелье +
    0,           // 0 философский камень
    338367744,   // 1 Вытяжка зародыше апаллала
    1498617088,  // 2 Хребты Рыбы-Льва
    666047744,   // 3 Сок мурлакомля
    269948160,   // 4 Стандартный ингридиент Н
    0,           // 5 Морские жёлуди
    0,           // 6 Безумные Многоножки
    0,           // 7 Лёд со дна серебристого озера
  },
  {
    //  0 Зелье Плавунчика или Морское зелье +
    0,           // 0 философский камень
    338367744,   // 1 Вытяжка зародыше апаллала
    1498617088,  // 2 Хребты Рыбы-Льва
    666047744,   // 3 Сок мурлакомля
    269948160,   // 4 Стандартный ингридиент Н
    0,           // 5 Морские жёлуди
    0,           // 6 Безумные Многоножки
    0,           // 7 Лёд со дна серебристого озера
  },
  {
    //  0 Зелье Плавунчика или Морское зелье +
    0,           // 0 философский камень
    338367744,   // 1 Вытяжка зародыше апаллала
    1498617088,  // 2 Хребты Рыбы-Льва
    666047744,   // 3 Сок мурлакомля
    269948160,   // 4 Стандартный ингридиент Н
    0,           // 5 Морские жёлуди
    0,           // 6 Безумные Многоножки
    0,           // 7 Лёд со дна серебристого озера
  }


};

byte selected_recipe;


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

  startstep = 0;
  Stepper_calibrated();
  // Stepper_calibrated_revolver();
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;
  uint32_t cardid;


  if (millis() - timing > 50) {

    timing = millis();
    reader == 7 ? reader = 0 : reader++;
    Uidtable[reader] = ReadUid(reader);
    Serial.print(" cardid : ");        //  "Сообщение: "
    Serial.println(Uidtable[reader]);  //  "Сообщение: "
    if (startstep >= 1) {
      lathent();
    }
  }



  while (stepper.tick()) {
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

  if ((newCode == 1111000005) || (newCode == 16726215)) {
    pcf.digitalWrite(4, HIGH);  // turn LED off by turning off sinking transistor
    delay(1000);
    pcf.digitalWrite(4, LOW);  // turn LED on by sinking current to ground
    newCode = 0;
    Serial.print("Open 4");
  }

  if (((newCode == 1111000004) || (newCode == 16716015)) && startstep != 2) {
    startstep = 1;
    newCode = 0;
    Serial.print("startstep ");
    Serial.print(startstep);
    strip.fill(mRGB(0, 222, 222));
    strip.show();
  }

  if (startstep == 1) {
    startstep = 2;
    Serial.print("startstep ");
    Serial.print(startstep);
    positionnow = 1700;
    stepper.setTarget(positionnow);
    delay(100);
    movestop = 0;
  }

  if (startstep == 2) {

    for (size_t i = 0; i < sizeof(potions) / sizeof(potions[0]); i++) {
      if (Uidtable[7] == potions[i][7] && Uidtable[7] != 0) {
        strip.fill(0, 7, mRGB(0, 0, 222)); // blue
        pcf.digitalWrite(1, HIGH);  // turn LED on by sinking current to ground

        selected_recipe = i;
        movestop = 1;
        startstep = 3;
      }
    }

    strip.show();
    if (movestop == 0) {

      positionnow = positionnow + 30;
      if (positionnow > 2500) {
        startstep = 4;
      }
      stepper.setTarget(positionnow);
    }
  }

  // if recipe card found and recognized
  if (startstep == 3) {
    comparisonuid = 0;
    for (byte t = 0; t < 7; t++) {
      if (Uidtable[t] == potions[selected_recipe][t]) {
        strip.set(t, mRGB(111, 222, 222)); // light blue
        comparisonuid++;
      } else if (Uidtable[t] != 0) {
        strip.set(t, mRGB(222, 0, 0)); // red
      } else {
        strip.fill(t, 7, mRGB(0, 0, 222)); // blue
      }
    }
    strip.show();

    if (comparisonuid == 7) {
      delay(2000);
      strip.fill(mRGB(0, 222, 222)); // light blue
      strip.show();

      pcf.digitalWrite(selected_recipe + 2, HIGH);  // turn LED off by turning off sinking transistor
      delay(1000);
      pcf.digitalWrite(1, LOW);                    // turn LED on by sinking current to ground
      pcf.digitalWrite(selected_recipe + 2, LOW);  // turn LED on by sinking current to ground
      delay(1000);

      delay(5000);
      Serial.print(" !!!!!! FINISH!!!!!!!!!!!!!!!!!!!!  ");
      movestop = 0;
      comparisonuid++;
      startstep = 4;
    }
  }

  if (startstep == 4) {
    Serial.print("startstep ");
    Serial.println(startstep);
    positionnow = 6500;
    stepper.setTarget(positionnow);
    //delay(100);
    if (stepper.getCurrent() > 6000) {
      startstep++;
    }
  }

  if (startstep == 5) {
    Stepper_calibrated();
    startstep = 0;
    movestop = 1;
    strip.clear();
    strip.show();
    startstep = 0;
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

void Stepper_calibrated() {
  stepper.autoPower(true);
  pinMode(12, INPUT_PULLUP);  // кнопка на D12 и GND
  stepper.setRunMode(KEEP_SPEED);
  stepper.setSpeedDeg(-60);  // медленно крутимся НАЗАД
  while (digitalRead(12)) {
    stepper.tick();
  }

  stepper.reset();
  stepper.setSpeedDeg(-20);
  while (stepper.getCurrent() != -390) {
    stepper.tick();
  }

  stepper.reset();
  stepper.setRunMode(FOLLOW_POS);
  stepper.stop();
}

void Stepper_calibrated_revolver() {
  stepper2.autoPower(true);
  stepper2.setRunMode(KEEP_SPEED);
  stepper2.setSpeedDeg(-70);  // медленно крутимся НАЗАД

  // пока кнопка не нажата
  while (digitalRead(11)) {
    stepper2.tick();
    // yield();	// для esp8266
  }

  stepper2.reset();
  stepper2.setSpeedDeg(60);
  while (stepper2.getCurrent() != 1550) {
    stepper2.tick();
  }

  stepper2.reset();
  stepper2.setSpeedDeg(-30);  // медленно крутимся НАЗАД

  // пока кнопка не нажата
  while (digitalRead(11)) {
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


void tcaselect(uint8_t i2c_bus) {
  if (i2c_bus > 7) return;
  Wire.beginTransmission(0x70);
  Wire.write(1 << i2c_bus);
  Wire.endTransmission();
}


void lathent() {
  static int count = 0;


  for (int i = 7; i < 34; i++) {
    count += 2;
    strip.set(i, CRGBtoData(ColorFromPalette(fire_p, inoise8(i * 25, count), 255, LINEARBLEND)));
  }
}
