/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>

#include <FastLED.h>

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "DYPlayerArduino.h"

#include <iarduino_IR_RX.h>
iarduino_IR_RX IR(6);  // declaring IR object & its pin

#define SS_PIN 10
#define RST_PIN 9

const byte MOTOR_PIN = 5;
byte motor_value = 0;

// Initialise the player on software serial port.
SoftwareSerial SoftSerial(2, 3);
DY::Player player(&SoftSerial);

MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];

String rfid_uid = "", rfid_uid_prev = "";
byte rfid_data;

// row is quiz selection, column is language selection (RU, LV, EN)
const byte GAME_COUNT = 2;
const byte LANGUAGE_COUNT = 3;
const byte GAME_MODE_COUNT = 3;

const String FORCE_STOP_CARD = "048e8e1a237380";
const String REPEAT_QUESTION_CARD = "047a8e1a237380";

const byte QUESTION_COUNT = 20;
byte questions[QUESTION_COUNT];
byte last_question_played = 0;

bool game_in_progress = false;
byte selected_game = 0;
byte selected_language = 0;
byte game_mode = 2;  // 0 - one random question on start, 1 - quest, 2 - quiz

bool question_played = false;

static uint32_t rebootTimer = millis();

byte newRFIDcardtimer = 0;

// LED strip constants and variables
const byte BACKLIGHT_LED_COUNT = 20;
CRGB leds[QUESTION_COUNT + BACKLIGHT_LED_COUNT];
CRGBPalette16 gPal;
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING 20
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120
#define BRIGHTNESS 50
#define FRAMES_PER_SECOND 50

void setup() {
  Serial.begin(115200);
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();  // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  // Setting up sound
  player.begin();
  player.setVolume(25);  // 50% Volume
  player.setCycleMode(DY::PlayMode::OneOff);
  byte sd_card_status = (int16_t)player.getPlayingDevice();
  if (sd_card_status == 1) {
    Serial.println("SD card read succesfully");
  } else {
    Serial.println("SD card read failed");
  }
  Serial.print("Number of sound records found: ");
  Serial.println((int16_t)player.getSoundCount());

  randomSeed(analogRead(0));

  // Setting up IR receiver
  IR.begin();

  // Init LED strip, blink ligths LED green one by one
  FastLED.addLeds<NEOPIXEL, 4>(leds, QUESTION_COUNT + BACKLIGHT_LED_COUNT);  // rgb ordering is assumed
  for (byte i = 0; i < QUESTION_COUNT + 1 + BACKLIGHT_LED_COUNT; i++) {
    leds[i] = CRGB::Green;
    FastLED.show();
    delay(100);
    leds[i] = CRGB::Black;
    FastLED.show();
  }

  FastLED.setBrightness(BRIGHTNESS);

  // color palette to be used for backlight
  gPal = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

  Serial.println("Initialization complete");
}

void loop() {
  // 7 lines below this are needed to fight RFID freeze. Solution found here: https://alexgyver.ru/arduino-rfid/
  if (millis() - rebootTimer >= 1000) {  // Таймер с периодом 1000 мс
    rebootTimer = millis();              // Обновляем таймер
    digitalWrite(RST_PIN, HIGH);         // Сбрасываем модуль
    delayMicroseconds(2);                // Ждем 2 мкс
    digitalWrite(RST_PIN, LOW);          // Отпускаем сброс
    rfid.PCD_Init();                     // Инициализируем заново
  }

  random16_add_entropy(random());

  if (newRFIDcardtimer < 10) {
    newRFIDcardtimer++;
  }
  readRFID();
  if (newRFIDcardtimer >= 10) {
    rfid_uid = "";
  }

  if (IR.check()) {  // Если в буфере имеются данные, принятые с пульта (была нажата кнопка)
    Serial.print("IR data received: ");
    Serial.println(IR.data);    // Выводим код нажатой кнопки
    if (IR.data == 16748655) {  // vol+ button
      if (selected_language + 1 == LANGUAGE_COUNT) {
        selected_language = 0;
      } else {
        selected_language++;
      }
      question_played = false;
      Serial.print("Selected language (0 - RU, 1 - EN, 2 - LV): ");
      Serial.println(selected_language);
    } else if (IR.data == 16754775) {  // vol- button
      if (selected_language == 0) {
        selected_language = LANGUAGE_COUNT - 1;
      } else {
        selected_language--;
      }
      question_played = false;
      Serial.print("Selected language (0 - RU, 1 - EN, 2 - LV): ");
      Serial.println(selected_language);
    } else if (IR.data == 16761405) {  // forward button
      if (selected_game + 1 == GAME_COUNT) {
        selected_game = 0;
      } else {
        selected_game++;
      }
      Serial.print("Selected game: ");
      Serial.println(selected_game);
    } else if (IR.data == 16712445) {  // backward button
      if (selected_game == 0) {
        selected_game = GAME_COUNT - 1;
      } else {
        selected_game--;
      }
      Serial.print("Selected game: ");
      Serial.println(selected_game);
    } else if (IR.data == 16736925) {  // mode button
      if (game_mode == GAME_MODE_COUNT - 1) {
        game_mode = 0;
      } else {
        game_mode++;
      }
      Serial.print("Game mode (0 - random, 1 - quest, 2 - quiz): ");
      Serial.println(game_mode);
    } else if (IR.data == 16716015) {  // button 4
      game_in_progress = true;
      Serial.print("Starting game: ");
      Serial.println(selected_game);
      Serial.print("Selected language (0 - RU, 1 - EN, 2 - LV): ");
      Serial.println(selected_language);
      Serial.println();

      init_and_shuffle_questions();

      // reading rules
      Serial.println("Playing rules recording");
      char path[] = "";
      sprintf(path, "/00/00/%02d.mp3", selected_language);
      Serial.print("Playing ");
      Serial.println(path);
      player.playSpecifiedDevicePath(DY::Device::Sd, path);
      while (player.checkPlayState() == DY::PlayState::Playing) {
        if (motor_value < 90) {
          motor_value++;
          analogWrite(MOTOR_PIN, motor_value);
          delay(20);
        }

        Fire2012WithPalette();  // run simulation frame, using palette colors
        FastLED.show();         // display this frame
        FastLED.delay(1000 / FRAMES_PER_SECOND);
      }
    }
  }

  if (game_in_progress) {
    Fire2012WithPalette();  // run simulation frame, using palette colors
    FastLED.show();         // display this frame
    FastLED.delay(1000 / FRAMES_PER_SECOND);

    if (!question_played) {
      // change backlight palette to blue
      gPal = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

      // light up current question
      leds[BACKLIGHT_LED_COUNT + last_question_played] = CRGB::Blue;
      FastLED.show();

      // reading question
      Serial.print("Playing question: ");
      Serial.println(questions[last_question_played]);

      char path[] = "";
      sprintf(path, "/%02d/%02d/%02d.mp3", selected_game + 1, selected_language, questions[last_question_played]);
      Serial.print("Playing ");
      Serial.println(path);
      player.playSpecifiedDevicePath(DY::Device::Sd, path);
      // while (player.checkPlayState() == DY::PlayState::Playing) {}

      question_played = true;
    }

    if (question_played && is_answer_card() && (rfid_uid != rfid_uid_prev) && (rfid_uid != "")) {
      newRFIDcardtimer++;
      if (rfid_data == questions[last_question_played] + QUESTION_COUNT * selected_game) {
        Serial.print("Answer presented: ");
        Serial.println(rfid_data);
        Serial.println("Correct answer!");

        // Change backlight palette to green-yellow
        gPal = CRGBPalette16(CRGB::Black, CRGB::GreenYellow, CRGB::Green, CRGB::DarkGreen);

        // turn the light corresponding to question green
        leds[BACKLIGHT_LED_COUNT + last_question_played] = CRGB::Green;
        FastLED.show();

        // play recording
        int k = random(1, 6);  // Generate a random index from 1 to 5
        char path[] = "";
        sprintf(path, "/00/01/%02d/0/%01d.mp3", selected_language, k);
        Serial.print("Playing ");
        Serial.println(path);
        player.playSpecifiedDevicePath(DY::Device::Sd, path);
        while (player.checkPlayState() == DY::PlayState::Playing) {
          if (motor_value > 90) {
            motor_value--;
            analogWrite(MOTOR_PIN, motor_value);
            delay(20);
          }

          Fire2012WithPalette();  // run simulation frame, using palette colors
          FastLED.show();         // display this frame
          FastLED.delay(1000 / FRAMES_PER_SECOND);
        }

        last_question_played++;
        question_played = false;
      } else {
        Serial.print("Answer presented: ");
        Serial.println(rfid_data);
        Serial.println("Wrong answer!");

        // Change backlight palette to red-yellow
        gPal = CRGBPalette16(CRGB::Black, CRGB::Yellow, CRGB::Orange, CRGB::Red);

        // turn the light corresponding to question red
        leds[BACKLIGHT_LED_COUNT + last_question_played] = CRGB::Red;
        FastLED.show();

        // play recording
        int k = random(1, 6);  // Generate a random index from 1 to 5
        char path[] = "";
        sprintf(path, "/00/01/%02d/1/%01d.mp3", selected_language, k);
        Serial.print("Playing ");
        Serial.println(path);
        player.playSpecifiedDevicePath(DY::Device::Sd, path);
        while (player.checkPlayState() == DY::PlayState::Playing) {
          if (motor_value < 105) {
            motor_value++;
            analogWrite(MOTOR_PIN, motor_value);
            delay(20);
          }
          Fire2012WithPalette();  // run simulation frame, using palette colors
          FastLED.show();         // display this frame
          FastLED.delay(1000 / FRAMES_PER_SECOND);
        }

        if (game_mode == 2) {
          last_question_played++;
          question_played = false;
        } else if (game_mode == 1) {
          leds[BACKLIGHT_LED_COUNT + last_question_played] = CRGB::Blue;
          FastLED.show();
          gPal = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);
        }
      }
    }

    if (last_question_played >= QUESTION_COUNT) {
      Serial.println("Game finished");
      end_game();
    }

    if ((rfid_uid == FORCE_STOP_CARD) && (rfid_uid != rfid_uid_prev)) {
      Serial.println("Game stopped");
      end_game();
    } else if ((rfid_uid == REPEAT_QUESTION_CARD) && (rfid_uid != rfid_uid_prev)) {
      question_played = false;
      rfid_uid = "";
    }
  }

  rfid_uid_prev = rfid_uid;
}

void readRFID() {
  String hexString = "";
  // 3 variables below to read data from RFID
  uint8_t dataBlock[18];             // buffer to read RFID data into
  uint8_t size = sizeof(dataBlock);  // buffer size
  MFRC522::StatusCode status;

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Verify if the NUID has been read
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  newRFIDcardtimer = 0;

  // if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3]) {
  //   Serial.println(F("A new card has been detected."));

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  for (int i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      // If the value is less than 0x10, add a leading zero for better formatting
      hexString += "0";
    }
    hexString += String(rfid.uid.uidByte[i], HEX);
  }
  // } else Serial.println(F("Card read previously."));

  // reading data from block 15
  status = rfid.MIFARE_Read(15, dataBlock, &size);  // Reading block 15 into buffer
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Read error");  // If error occurs, it will be printed
    return;
  }

  rfid_data = dataBlock[0];

  // // Halt PICC
  // rfid.PICC_HaltA();

  // // Stop encryption on PCD
  // rfid.PCD_StopCrypto1();

  rfid_uid = hexString;
}

void init_and_shuffle_questions() {
  // Initialize questions
  for (byte i = 0; i < QUESTION_COUNT; i++) {
    questions[i] = i + 1;
  } 

  // Shuffle questions using Fisher-Yates algorithm
  for (int i = QUESTION_COUNT - 1; i > 0; i--) {
    int j = random(i + 1);  // Generate a random index from 0 to i
    int temp = questions[i];
    questions[i] = questions[j];
    questions[j] = temp;

    for (byte i = 0; i < QUESTION_COUNT; i++) {
      Serial.print(questions[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  //   // Uncomment below to print shuffled question array
  //   for (byte i = 0; i < QUESTION_COUNT; i++) {
  //     Serial.print(questions[i]);
  //     Serial.print(" ");
  //   }
  //   Serial.println();
  // }
}

// helper function to check whether last read RFID UID is answer card
bool is_answer_card() {
  if (rfid_uid == FORCE_STOP_CARD || rfid_uid == REPEAT_QUESTION_CARD) return false;
  return true;
}

void end_game() {
  game_in_progress = false;
  rfid_uid = "";

  // play recording
  // give out coins

  // turn off all backlights
  for (byte i = 0; i < BACKLIGHT_LED_COUNT; i++) {
    leds[i] = CRGB::Black;
  }

  while (last_question_played > 0) {
    leds[BACKLIGHT_LED_COUNT + last_question_played - 1] = CRGB::Black;
    FastLED.show();
    delay(100);
    last_question_played--;
  }

  leds[BACKLIGHT_LED_COUNT] = CRGB::Black;
  FastLED.show();

  while (motor_value > 0) {
    motor_value--;
    analogWrite(MOTOR_PIN, motor_value);
    delay(20);
  }
}

void Fire2012WithPalette() {
  // Array of temperature readings at each simulation cell
  static uint8_t heat[BACKLIGHT_LED_COUNT];

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < BACKLIGHT_LED_COUNT; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / BACKLIGHT_LED_COUNT) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = BACKLIGHT_LED_COUNT - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (random8() < SPARKING) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < BACKLIGHT_LED_COUNT; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    uint8_t colorindex = scale8(heat[j], 240);
    CRGB color = ColorFromPalette(gPal, colorindex);
    int pixelnumber;
    pixelnumber = j;
    leds[pixelnumber] = color;
  }
}