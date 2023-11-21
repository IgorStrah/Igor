
#include <MFRC522v2.h>
#include <MFRC522DriverI2C.h>
#include <MFRC522Debug.h>


    int serNum0;
    int serNum1;
    int serNum2;
    int serNum3;
    int serNum4;

const uint8_t customAddress = 0x28;
const uint8_t customAddress1 = 0x3b;
TwoWire &customI2C = Wire;

MFRC522DriverI2C driver{customAddress, customI2C}; 

MFRC522 mfrc522{driver};


MFRC522DriverI2C driver1{customAddress1, customI2C}; 

MFRC522 mfrc522_1{driver1};



void setup() {
  Serial.begin(115200); // Initialize serial communications with the PC for debugging.
  while (!Serial);      // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  mfrc522.PCD_Init();   
  mfrc522_1.PCD_Init();// Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of 
   MFRC522Debug::PCD_DumpVersionToSerial(mfrc522_1, Serial);	// Show details of PCD - MFRC522 Card Reader details.
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
	if ( mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()) {
	
        Serial.print("1Card number:");
        Serial.println(mfrc522.uid.uidByte[1]);
	}


	if ( mfrc522_1.PICC_IsNewCardPresent() || mfrc522_1.PICC_ReadCardSerial()) {
    Serial.println("2Card number:");
          Serial.print(mfrc522_1.uid.uidByte[1]);
          Serial.println(", ");
	}



    
}