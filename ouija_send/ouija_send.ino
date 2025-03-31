
//RF24 radio(9, 10);  // CE, CSN
//const byte serverAddress[6] = "Server";
//const byte clientAddress[6] = "Client";


// CLIENT CODE
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <MFRC522.h>

#define VIBRATION_PIN 2
#define SS_PIN 10
#define RST_PIN 9


RF24 radio(9, 10);  // CE, CSN
const byte serverAddress[6] = "Server";
const byte clientAddress[6] = "Client";

MFRC522 mfrc522(SS_PIN, RST_PIN);

unsigned long vibEndTime = 0;
int vibValue = 0;

void setup() {
    Serial.begin(115200);
    SPI.begin();
    mfrc522.PCD_Init();
    pinMode(VIBRATION_PIN, OUTPUT);
    analogWrite(VIBRATION_PIN, 0);
    
    radio.begin();
    radio.openWritingPipe(serverAddress);
    radio.openReadingPipe(1, clientAddress);
    radio.setPALevel(RF24_PA_LOW);
    radio.startListening();
}

void loop() {
    static char uidStr[16] = "";
    
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        memset(uidStr, 0, sizeof(uidStr));
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            sprintf(uidStr + strlen(uidStr), "%02X", mfrc522.uid.uidByte[i]);
        }
        Serial.print("Read UID: "); Serial.println(uidStr);
        
        radio.stopListening();
        radio.write(uidStr, sizeof(uidStr));
        radio.startListening();
    }
    
    if (radio.available()) {
        struct {
            int vib;
            int timevib;
        } data;
        
        radio.read(&data, sizeof(data));
        Serial.print("Received vib: "); Serial.print(data.vib);
        Serial.print(" | timevib: "); Serial.println(data.timevib);
        
        vibValue = data.vib;
        vibEndTime = millis() + data.timevib;
        analogWrite(VIBRATION_PIN, vibValue);
    }
    
    if (vibValue > 0 && millis() >= vibEndTime) {
        analogWrite(VIBRATION_PIN, 0);
        vibValue = 0;
    }
}
