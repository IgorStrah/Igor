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

String RFID = "0";

// row is quiz selection, column is language selection (RU, LV, EN)
const byte GAME_COUNT = 2;
const byte LANGUAGE_COUNT = 3;
String quiz_cards[GAME_COUNT][LANGUAGE_COUNT] = {
  { "040d8f1a237380", "04118f1a237380", "04198f1a237380" },
  { "04868e1a237380", "04828e1a237380", "047e8e1a237380" }
};

String force_stop_card = "048e8e1a237380";
String repeat_question_card = "047a8e1a237380";

const byte QUESTION_COUNT = 20;
byte* questions[QUESTION_COUNT];
byte last_question_played = 0;

bool game_in_progress = false;
byte selected_game;
byte selected_language;

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
    byte* value = new byte(i);  // Allocate memory for the byte value
    questions[i] = value;       // Assign the pointer to the array element
  }
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

  RFID = readRFID();

  if (!game_in_progress) {
    for (byte i = 0; i < GAME_COUNT; i++) {
      for (byte j = 0; j < LANGUAGE_COUNT; j++) {
        if (RFID == quiz_cards[i][j]) {
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
    // reading question
    Serial.println(last_question_played);
    Serial.print("Playing question: ");
    // play recording

    last_question_played++;

    if (last_question_played >= QUESTION_COUNT) {
      Serial.println("Game finished");
      game_in_progress = false;
      RFID = "";
      // play recording
      // give out coins
      // reduce coin counter to 0
      shuffle_questions();
      Serial.println("Questions shuffled");
      last_question_played = 0;
      // turn off all lights
    }

    if (RFID == force_stop_card) {
      game_in_progress = false;
      Serial.println("Game stopped");
    } else if (repeat_question_card) {
      // play last question again
    }
  }
}

String readRFID() {
  String hexString = "";
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
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

  Serial.println(hexString);
  Serial.println();
  // } else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  return hexString;
}

void shuffle_questions() {
  // Shuffle questions using Fisher-Yates algorithm
  for (int i = QUESTION_COUNT - 1; i > 0; --i) {
    int j = random(0, i + 1);  // Generate a random index from 0 to i
    // Swap the elements at indices i and j
    byte* temp = questions[i];
    questions[i] = questions[j];
    questions[j] = temp;
  }
}