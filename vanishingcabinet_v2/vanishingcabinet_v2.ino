#define NUMLEDS 35   // кол-во светодиодов
#define STRIP_PIN 3  // пин ленты

#include <Adafruit_PCF8574.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <IRremote.hpp>
#include <microLED.h>  // подключаем библу
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2813, ORDER_GRB, CLI_AVER> strip;

#define PN532_IRQ (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield
byte reader;
int NB_NFC_READER = 7;
unsigned long newCode;
unsigned long code;
unsigned long rfid_reader_timer, IR_timer;
#include <FastLEDsupport.h>

bool is_lantern_on = false;

int8_t objects_present[8] = { 0 };
int8_t objects_expected[8] = { 0 };
int8_t spells_expected[4] = { 0 };
int8_t door_nr;
int8_t inner_effect = 0;
int8_t objects_expected_count = 0;
int8_t objects_present_count = 0;
int8_t spells_expected_count = 0;
int8_t spells_present_count = 0;
int8_t current_spell_nr = 0;
bool door_opened = false;

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
byte data[4] = { 0 };

bool recipe_present = false;
bool waiting_for_spells = false;

long remote_signals[9] = { 16724175, 16718055, 16743045, 16716015, 16726215, 16734885, 16728765, 16730805, 16732845 };

Adafruit_PCF8574 pcf;

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

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
  Wire.beginTransmission(0x70);
  Wire.write(1 << 0);
  Wire.endTransmission();
  nfc.begin();
}


void loop(void) {
  if (millis() - rfid_reader_timer > 50) {
    rfid_reader_timer = millis();

    for (byte reader = 0; reader < 8; reader++) {
      read_rfid_data(reader);
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

    if (((newCode == 1111000001) || (newCode == 16724175)) && (millis() - IR_timer > 1000) && !recipe_present) {
      IR_timer = millis();

      if (is_lantern_on) {
        clear_strip(7, 34);
      } else {
        Serial.println("Turning lantern on");
        for (byte i = 7; i < 34; i++) {
          strip.set(i, mRGB(0, 0, 222));  // blue
        }
        strip.show();
      }

      is_lantern_on = !is_lantern_on;
    }

    if (newCode == 16736925) {  // "mode" button
      Serial.println("Opening all doors");
      for (byte i = 0; i <= 8; i++) {
        open_door(i);
      }
    }

    if (waiting_for_spells && (millis() - IR_timer > 1000)) {
      for (byte i = 0; i < 9; i++) {
        if (((newCode == remote_signals[i]) || (newCode == 1111000000 + i + 1)) && (spells_expected[current_spell_nr] == i + 1)) {
          strip.set(2 + current_spell_nr, mRGB(0, 230, 60));  // green
          current_spell_nr++;
          spells_present_count++;

        } else {
          if (current_spell_nr < spells_expected_count) {
            strip.set(2 + current_spell_nr, mRGB(222, 0, 0));  // red
          }
        }
      }
      strip.show();
    }

    newCode = 0;
    IrReceiver.resume();
  }

  if (recipe_present) {
    if (waiting_for_spells) {
      if (spells_expected_count == spells_present_count) {
        if (!door_opened) {
          open_door(door_nr);
          door_opened = true;
        }
      }
    } else {
      if (objects_present_count == objects_expected_count) {
        waiting_for_spells = true;

        strip.clear();
        // turn lantern on
        for (byte i = 7; i < 34; i++) {
          strip.set(i, mRGB(0, 222, 222));  // purple
        }
        for (byte i = 2 + spells_present_count; i < 2 + spells_expected_count; i++) {
          strip.set(i, mRGB(222, 0, 222));  // purple
        }
        strip.show();
      } else {
        // turn on lantern
        if (!is_lantern_on) {
          for (byte i = 7; i < 34; i++) {
            strip.set(i, mRGB(0, 0, 222));  // blue
          }
          is_lantern_on = true;
        }

        objects_present_count = 0;
        for (byte i = 0; i < 7; i++) {
          if (objects_present[i] != 0) {
            if (objects_expected[i] == objects_present[i]) {
              strip.set(i, mRGB(0, 230, 60));  // green
              objects_present_count++;
            } else {
              strip.set(i, mRGB(222, 0, 0));  // red
            }
          } else {
            strip.set(i, mRGB(0, 0, 222));  // blue
          }
          strip.show();
        }
      }
    }

    if (objects_present_count == objects_expected_count) {
      waiting_for_spells = true;

      strip.clear();
      // turn lantern on
      for (byte i = 7; i < 34; i++) {
        strip.set(i, mRGB(0, 222, 222));  // purple
      }
      for (byte i = 2; i < 2 + spells_expected_count; i++) {
        strip.set(i, mRGB(222, 0, 222));  // purple
      }
      strip.show();
    }
  }

  Serial.print("Objects present: ");
  for (byte i = 0; i < 7; i++) {
    Serial.print(objects_present[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("Objects expected: ");
  for (byte i = 0; i < 7; i++) {
    Serial.print(objects_expected[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("Spells expected: ");
  for (byte i = 0; i < 4; i++) {
    Serial.print(spells_expected[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("Expected spell count: ");
  Serial.println(spells_expected_count);

  Serial.print("Present spell count: ");
  Serial.println(spells_present_count);

  Serial.print("Door number: ");
  Serial.println(door_nr);

  Serial.print("Inner effect: ");
  Serial.println(inner_effect);

  Serial.print("Expected object count: ");
  Serial.println(objects_expected_count);

  Serial.print("Present object count: ");
  Serial.println(objects_present_count);
}

void read_rfid_data(byte numReader) {
  uint8_t success;

  tcaselect(numReader);
  delay(10);
  nfc.begin();
  delay(50);

  // Serial.print("Reader: ");
  // Serial.println(numReader);
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);

  if (success) {
    // Display some basic information about the card
    // Serial.println("Found an ISO14443A card");
    // Serial.print("  UID Length: ");
    // Serial.print(uidLength, DEC);
    // Serial.println(" bytes");
    // Serial.print("  UID Value: ");
    // nfc.PrintHex(uid, uidLength);
    // Serial.println("");

    if (numReader == 7) {
      read_rfid_data_block(8);
      for (byte i = 0; i < 4; i++) {
        objects_expected[i] = data[i];
      }

      read_rfid_data_block(9);
      for (byte i = 0; i < 4; i++) {
        objects_expected[i + 4] = data[i];
      }

      read_rfid_data_block(10);
      for (byte i = 0; i < 4; i++) {
        spells_expected[i] = data[i];
      }

      read_rfid_data_block(12);
      door_nr = data[0];
      inner_effect = data[1];

      if (!recipe_present) {
        recipe_present = true;
        objects_expected_count = 0;
        for (byte i = 0; i < 7; i++) {
          if (objects_expected[i] > 0) {
            objects_expected_count++;
          }
        }

        spells_expected_count = 0;
        for (byte i = 0; i < 4; i++) {
          if (spells_expected[i] > 0) {
            spells_expected_count++;
          }
        }

        // turn lantern on
        Serial.println("Recipe found");
        for (byte i = 7; i < 34; i++) {
          strip.set(i, mRGB(0, 0, 222));  // blue
        }
        strip.show();
      }
    } else {
      read_rfid_data_block(13);
      objects_present[numReader] = data[0];
    }
  } else {
    if (numReader == 7) {
      clear_variables();
      clear_strip(0, 7);
    } else {
      objects_present[numReader] = 0;
    }
  }
}

void read_rfid_data_block(byte datablock) {
  uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  uint8_t success;

  if (uidLength == 4) {
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, datablock, 0, keya);
    if (success) {
      uint8_t data_temp[16];
      success = nfc.mifareclassic_ReadDataBlock(datablock, data_temp);
      if (success) {
        for (byte i = 0; i < 4; i++) {
          data[i] = data_temp[i];
        }

      } else {
        Serial.println("Ooops ... unable to read the requested block.  Try another key?");
      }
    } else {
      Serial.println("Ooops ... authentication failed: Try another key?");
    }
  }

  if (uidLength == 7) {
    uint8_t data_temp[32];
    success = nfc.mifareultralight_ReadPage(datablock, data_temp);
    if (success) {
      for (byte i = 0; i < 4; i++) {
        data[i] = data_temp[i];
      }
    } else {
      Serial.println("Ooops ... unable to read the requested page!?");
    }
  }

  if (!success) {
    for (byte i = 0; i < 4; i++) {
      data[i] = 0;
    }
  } else {
    // Uncomment to print out read data
    // Serial.print("Data: ");
    // for (byte i = 0; i < 4; i++) {
    //   Serial.print(data[i]);
    //   Serial.print(" ");
    // }
    // Serial.println();
  }
}

void tcaselect(uint8_t i2c_bus) {
  if (i2c_bus > 7) return;
  Wire.beginTransmission(0x70);
  Wire.write(1 << i2c_bus);
  Wire.endTransmission();
}

void clear_strip(byte from, byte to) {
  for (byte i = from; i < to; i++) {
    strip.set(i, CRGB::Black);
  }
  strip.show();
}

void open_door(byte door_nr) {
  pcf.digitalWrite(door_nr, HIGH);
  delay(1000);
  pcf.digitalWrite(door_nr, LOW);
  delay(1000);
}

void clear_variables() {
  // clearing variables
  for (byte i = 0; i < 7; i++) {
    objects_expected[i] = 0;
  }
  for (byte i = 0; i < 4; i++) {
    spells_expected[i] = 0;
  }
  door_nr = 0;
  inner_effect = 0;
  objects_expected_count = 0;
  recipe_present = false;
  spells_present_count = 0;
  current_spell_nr = 0;
  door_opened = false;
  waiting_for_spells = false;
}