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

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];

String rfid_uid = "", rfid_uid_prev = "";
byte rfid_data;

// row is quiz selection, column is language selection (RU, LV, EN)
const byte GAME_COUNT = 2;
const byte LANGUAGE_COUNT = 3;
const String QUIZ_CARDS[GAME_COUNT][LANGUAGE_COUNT] = {
  { "040d8f1a237380", "04118f1a237380", "04198f1a237380" },
  { "04868e1a237380", "04828e1a237380", "047e8e1a237380" }
};

const String FORCE_STOP_CARD = "048e8e1a237380";
const String REPEAT_QUESTION_CARD = "047a8e1a237380";

const byte QUESTION_COUNT = 20;
byte questions[QUESTION_COUNT];
byte last_question_played = 0;

bool game_in_progress = false;
byte selected_game;
byte selected_language;

bool question_played = false;

static uint32_t rebootTimer = millis();

void setup() {
  Serial.begin(115200);
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();  // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  randomSeed(analogRead(0));

  // Initialize questions
  for (int i = 0; i < QUESTION_COUNT; i++) {
    questions[i] = i;
  }

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

  readRFID();

  if (!game_in_progress && (rfid_uid_prev != rfid_uid)) {
    for (byte i = 0; i < GAME_COUNT; i++) {
      for (byte j = 0; j < LANGUAGE_COUNT; j++) {
        if (rfid_uid == QUIZ_CARDS[i][j]) {
          Serial.print("RFID card UID: ");
          Serial.println(rfid_uid);
          selected_game = i;
          selected_language = j;
          game_in_progress = true;
          Serial.println("Match found");
          Serial.print("Starting game: ");
          Serial.println(selected_game);
          Serial.print("Selected language (0 - RU, 1 - LV, 2 - EN): ");
          Serial.println(selected_language);
          Serial.println();

          shuffle_questions();

          // reading rules
          Serial.println("Playing rules recording");
          // play recording
        }
      }
    }
  }

  if (game_in_progress) {
    if (!question_played) {
      // reading question
      Serial.print("Playing question: ");
      Serial.println(questions[last_question_played]);
      // play recording
      question_played = true;
    }

    if (question_played && (rfid_uid != rfid_uid_prev) && is_answer_card()) {
      if (rfid_data == questions[last_question_played] + QUESTION_COUNT * selected_game) {
        Serial.print("Answer presented: ");
        Serial.println(rfid_data);
        Serial.println("Correct answer!");
        // play recording
        // grant coins
        // move coin counter up
        last_question_played++;
        question_played = false;
        rfid_uid = "";
      } else {
        Serial.print("Answer presented: ");
        Serial.println(rfid_data);
        Serial.println("Wrong answer!");
        // play recording
        // reduce coin count
        // move coin counter down
      }
    }

    if (last_question_played >= QUESTION_COUNT) {
      Serial.println("Game finished");
      game_in_progress = false;
      rfid_uid = "";
      // play recording
      // give out coins
      // reduce coin counter to 0
      shuffle_questions();
      Serial.println("Questions shuffled");
      last_question_played = 0;
      // turn off all lights
    }

    if (rfid_uid == FORCE_STOP_CARD) {
      game_in_progress = false;
      Serial.println("Game stopped");
    } else if (REPEAT_QUESTION_CARD) {
      // play last question again
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
    hexString = "";
    return;
  }

  // Verify if the NUID has been read
  if (!rfid.PICC_ReadCardSerial())
    return;

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

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  rfid_uid = hexString;
}

void shuffle_questions() {
  // Shuffle questions using Fisher-Yates algorithm
  for (int i = 0; i < QUESTION_COUNT; i++) {
    int j = random(0, i + 1);  // Generate a random index from 0 to i
    // Swap the elements at indices i and j
    byte temp = questions[i];
    questions[i] = questions[j];
    questions[j] = temp;
  }

  // Uncomment below to print shuffled question array
  // for (int i = 0; i < QUESTION_COUNT; i++) {
  //   Serial.print(questions[i]);
  //   Serial.print(" ");
  // }
  // Serial.println();
}

// helper function to check whether last read RFID UID is answer card
bool is_answer_card() {
  if (rfid_uid == FORCE_STOP_CARD || rfid_uid == REPEAT_QUESTION_CARD) return false;
  for (int i = 0; i < GAME_COUNT; i++) {
    for (int j = 0; j < LANGUAGE_COUNT; j++) {
      if (rfid_uid == QUIZ_CARDS[i][j]) return false;
    }
  }
  return true;
}