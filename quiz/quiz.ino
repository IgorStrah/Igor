
// for servo control
#include <Wire.h>
#define PCA9685_ADDR 0x40
#define MODE1 0x00
#define PRESCALE 0xFE
#define LED0_ON_L 0x06

#include <SPI.h>
#include <MFRC522.h>

#include <FastLED.h>

#include <Arduino.h>

#include "DYPlayerArduino.h"

#include <iarduino_IR_RX.h>
iarduino_IR_RX IR(6);  // declaring IR object & its pin

#define SS_PIN 10
#define RST_PIN 9

const byte MOTOR_PIN = 5;
byte motor_value = 0;

DY::Player player;

MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];

String rfid_uid = "", rfid_uid_prev = "";
byte rfid_data;

// row is quiz selection, column is language selection (RU, LV, EN)
const byte GAME_COUNT = 3;
const byte LANGUAGE_COUNT = 3;
const byte GAME_MODE_COUNT = 3;

const String REPEAT_QUESTION_CARD = "047a8e1a237380";
const String SKIP_QUESTION_CARD = "5372c2e4710001";

const byte MAX_QUESTION_COUNT = 21;
byte questions[MAX_QUESTION_COUNT];
byte last_question_played = 0;
byte question_count_in_game[GAME_COUNT] = { 20, 14, 20 };
byte question_count = 0;
byte coin_count = 0;
byte is_container_open = false;

bool game_in_progress = false;
byte selected_game = 0;
byte selected_language = 0;
byte game_mode = 2;  // 0 - one random question on start, 1 - quest, 2 - quiz

bool question_played = false;

static uint32_t rebootTimer = millis();

byte newRFIDcardtimer = 0;

// LED strip constants and variables
const byte BACKLIGHT_LED_COUNT = 20;
CRGB leds[MAX_QUESTION_COUNT + BACKLIGHT_LED_COUNT];
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
  // Serial.begin(9600);
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
    // Serial.println("SD card read succesfully");
  } else {
    // Serial.println("SD card read failed");
  }
  // Serial.print("Number of sound records found: ");
  // Serial.println((int16_t)player.getSoundCount());


  randomSeed(analogRead(0));

  // Serial.println("Setting up servos");
  Wire.begin();
  resetPCA9685();
  setPWMFreq(50);
  for (byte i = 0; i <= 1; i++) {
    setServoAngle(i, 0);
    delay(500);
    setServoAngle(i, 180);
    delay(500);
  }

  // Setting up IR receiver
  // Serial.println("Setting up IR receiver");
  IR.begin();

  // Init LED strip, blink ligths LED green one by one
  // Serial.println("Setting up LED strip");
  FastLED.addLeds<NEOPIXEL, 4>(leds, MAX_QUESTION_COUNT + BACKLIGHT_LED_COUNT);  // rgb ordering is assumed
  for (byte i = 0; i < MAX_QUESTION_COUNT + BACKLIGHT_LED_COUNT; i++) {
    leds[i] = CRGB::Green;
    FastLED.show();
    delay(100);
    leds[i] = CRGB::Black;
    FastLED.show();
  }

  FastLED.setBrightness(BRIGHTNESS);

  // color palette to be used for backlight
  gPal = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

  // Serial.println("Initialization complete");
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
    // Serial.print("IR data received: ");
    // Serial.println(IR.data);    // Выводим код нажатой кнопки
    if (IR.data == 16748655) {  // vol+ button
      if (selected_language + 1 == LANGUAGE_COUNT) {
        selected_language = 0;
      } else {
        selected_language++;
      }
      // Serial.println("Playing rules recording");
      char path[] = "";
      sprintf(path, "/00/03/%02d.mp3", selected_language);
      // Serial.print("Playing ");
      // Serial.println(path);
      player.playSpecifiedDevicePath(DY::Device::Sd, path);
      question_played = false;
      // Serial.print("Selected language (0 - RU, 1 - EN, 2 - LV): ");
      // Serial.println(selected_language);

    } else if (IR.data == 16754775) {  // vol- button
      if (selected_language == 0) {
        selected_language = LANGUAGE_COUNT - 1;
      } else {
        selected_language--;
      }
      question_played = false;
      // Serial.print("Selected language (0 - RU, 1 - EN, 2 - LV): ");
      // Serial.println(selected_language);
      char path[] = "";
      sprintf(path, "/00/03/%02d.mp3", selected_language);
      // Serial.print("Playing ");
      // Serial.println(path);
      player.playSpecifiedDevicePath(DY::Device::Sd, path);
      question_played = false;
      // Serial.print("Selected language (0 - RU, 1 - EN, 2 - LV): ");
      // Serial.println(selected_language);

    } else if ((IR.data == 16761405) && !game_in_progress) {  // forward button
      if (selected_game + 1 == GAME_COUNT) {
        selected_game = 0;
      } else {
        selected_game++;
      }
      // Serial.print("Selected game: ");
      // Serial.println(selected_game);
      char path[] = "";
      sprintf(path, "/%02d/00.mp3", selected_game + 1);
      // Serial.print("Playing ");
      // Serial.println(path);
      player.playSpecifiedDevicePath(DY::Device::Sd, path);

    } else if ((IR.data == 16712445) && !game_in_progress) {  // backward button
      if (selected_game == 0) {
        selected_game = GAME_COUNT - 1;
      } else {
        selected_game--;
      }
      // Serial.print("Selected game: ");
      // Serial.println(selected_game);
      char path[] = "";
      sprintf(path, "/%02d/00.mp3", selected_game + 1);
      // Serial.print("Playing ");
      // Serial.println(path);
      player.playSpecifiedDevicePath(DY::Device::Sd, path);

    } else if ((IR.data == 16736925) && !game_in_progress) {  // mode button
      if (game_mode == GAME_MODE_COUNT - 1) {
        game_mode = 0;
      } else {
        game_mode++;
      }
      // Serial.print("Game mode (0 - random, 1 - quest, 2 - quiz): ");
      // Serial.println(game_mode);
      char path[] = "";
      sprintf(path, "/00/02/%02d.mp3", game_mode);
      // Serial.print("Playing ");
      // Serial.println(path);
      player.playSpecifiedDevicePath(DY::Device::Sd, path);

    } else if ((IR.data == 16720605) && game_in_progress) {  // play/pause button
      // Serial.println("Game stopped");
      end_game();

    } else if (IR.data == 16738455) {
      if (is_container_open) {
        setServoAngle(1, 180);
      } else {
        setServoAngle(1, 0);
        delay(500);
        setServoAngle(0, 0);
        delay(500);
        setServoAngle(0, 180);
      }
      delay(500);
      is_container_open = !is_container_open;
    } else if ((IR.data == 16716015) || (IR.data == 1111000004) && !game_in_progress) {  // button 4
      game_in_progress = true;
      // Serial.print("Starting game: ");
      // Serial.println(selected_game);
      // Serial.print("Selected language (0 - RU, 1 - EN, 2 - LV): ");
      // Serial.println(selected_language);
      // Serial.println();

      if (game_mode == 0) {
        question_count = 1;
      } else {
        question_count = question_count_in_game[selected_game];
      }
      init_and_shuffle_questions();

      // reading rules
      // Serial.println("Playing rules recording");
      char path[] = "";
      if (selected_game == 1) {
        sprintf(path, "/%02d/%02d/00.mp3", selected_game + 1, selected_language);
      } else if (game_mode != 0) {
        sprintf(path, "/00/00/%02d.mp3", selected_language);
      }
      // Serial.print("Playing ");
      // Serial.println(path);
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
      // Serial.print("Playing question: ");
      // Serial.println(questions[last_question_played]);

      char path[] = "";
      sprintf(path, "/%02d/%02d/%02d.mp3", selected_game + 1, selected_language, questions[last_question_played]);
      // Serial.print("Playing ");
      // Serial.println(path);
      player.playSpecifiedDevicePath(DY::Device::Sd, path);
      // while (player.checkPlayState() == DY::PlayState::Playing) {}
      while (motor_value < 90) {
        motor_value++;
        analogWrite(MOTOR_PIN, motor_value);
        delay(20);
      }

      question_played = true;
    }

    if (question_played && (rfid_uid != REPEAT_QUESTION_CARD) && (rfid_uid != rfid_uid_prev) && (rfid_uid != "")) {
      newRFIDcardtimer++;
      if ((rfid_data == questions[last_question_played] + MAX_QUESTION_COUNT * selected_game) || (rfid_uid == SKIP_QUESTION_CARD)) {
        // Serial.print("Answer presented: ");
        // Serial.println(rfid_data);
        // Serial.println("Correct answer!");

        // Change backlight palette to green-yellow
        gPal = CRGBPalette16(CRGB::Black, CRGB::GreenYellow, CRGB::Green, CRGB::DarkGreen);

        // turn the light corresponding to question green
        leds[BACKLIGHT_LED_COUNT + last_question_played] = CRGB::Green;
        FastLED.show();

        // play recording
        int k = random(1, 6);  // Generate a random index from 1 to 5
        char path[] = "";
        sprintf(path, "/00/01/%02d/0/%01d.mp3", selected_language, k);
        // Serial.print("Playing ");
        // Serial.println(path);
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
        coin_count++;
        question_played = false;
      } else {
        // Serial.print("Answer presented: ");
        // Serial.println(rfid_data);
        // Serial.println("Wrong answer!");

        // Change backlight palette to red-yellow
        gPal = CRGBPalette16(CRGB::Black, CRGB::Yellow, CRGB::Orange, CRGB::Red);

        // turn the light corresponding to question red
        leds[BACKLIGHT_LED_COUNT + last_question_played] = CRGB::Red;
        FastLED.show();

        // play recording
        int k = random(1, 6);  // Generate a random index from 1 to 5
        char path[] = "";
        sprintf(path, "/00/01/%02d/1/%01d.mp3", selected_language, k);
        // Serial.print("Playing ");
        // Serial.println(path);
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

        if (game_mode != 1) {
          last_question_played++;
          question_played = false;
        } else {
          leds[BACKLIGHT_LED_COUNT + last_question_played] = CRGB::Blue;
          FastLED.show();
          gPal = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);
        }
      }
    }

    if (last_question_played >= question_count) {
      // Serial.println("Game finished");
      end_game();
    }

    if ((rfid_uid == REPEAT_QUESTION_CARD) && (rfid_uid != rfid_uid_prev)) {
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
    // Serial.println("Read error");  // If error occurs, it will be printed
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
  for (byte i = 0; i < MAX_QUESTION_COUNT; i++) {
    questions[i] = i + 1;
  }

  if (selected_game != 1) {  // question order must be preserved for game nr. 1
    // Shuffle questions using Fisher-Yates algorithm
    for (byte i = 0; i < question_count_in_game[selected_game] - 1; i++) {
      byte j = random(i, question_count_in_game[selected_game]);
      byte temp = questions[i];
      questions[i] = questions[j];
      questions[j] = temp;
    }
  }

  // Uncomment below to print shuffled question array
  for (byte i = 0; i < MAX_QUESTION_COUNT; i++) {
    // Serial.print(questions[i]);
    // Serial.print(" ");
  }
}

void end_game() {
  game_in_progress = false;
  rfid_uid = "";

  if (selected_game == 1) {
    // give out recipe
    setServoAngle(1, 0);
    delay(500);
    setServoAngle(1, 180);
    delay(500);
    coin_count--;
  } else {  // give out coins
    while (coin_count > 0) {
      setServoAngle(0, 0);
      delay(500);
      setServoAngle(0, 180);
      delay(500);
      coin_count--;
    }
  }

  coin_count = 0;

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

  game_mode = 0;
  selected_game = 2;
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

void resetPCA9685() {
  write8(PCA9685_ADDR, MODE1, 0x00);
  delay(10);
}

void setPWMFreq(float freq) {
  uint8_t prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;
  uint8_t oldmode = read8(PCA9685_ADDR, MODE1);
  uint8_t newmode = (oldmode & 0x7F) | 0x10;
  write8(PCA9685_ADDR, MODE1, newmode);
  write8(PCA9685_ADDR, PRESCALE, prescaleval);
  delay(5);
  write8(PCA9685_ADDR, MODE1, oldmode | 0xa1);
}

void setPWM(uint8_t num, uint16_t on, uint16_t off) {
  Wire.beginTransmission(PCA9685_ADDR);
  Wire.write(LED0_ON_L + 4 * num);
  Wire.write(on);
  Wire.write(on >> 8);
  Wire.write(off);
  Wire.write(off >> 8);
  Wire.endTransmission();
}

void setServoAngle(uint8_t num, uint16_t angle) {
  uint16_t pulse_wide = map(angle, 0, 180, 80, 240);
  setPWM(num, 0, pulse_wide);
  delay(500);
  setPWM(num, 0, 0);
}

uint8_t read8(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(addr, (uint8_t)1);
  return Wire.read();
}

void write8(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}