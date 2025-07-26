 
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#define RST_PIN D3  // Configurable, see typical pin layout above
#define SS_PIN D4   // Configurable, see typical pin layout above

const char* ssid = "LMT-5DA0";
const char* password = "IH7R9QCREC";
char uidBuffer[16] = "NoCard";  // Буфер для UID (если нет карты)

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
	Serial.begin(115200);  // Initialize serial communications with the PC
	while (!Serial)
		;                                 // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();                        // Init SPI bus
	mfrc522.PCD_Init();                 // Init MFRC522
	delay(4);                           // Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  WiFi.begin(ssid, password);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }



	ArduinoOTA.setHostname("esp12-ota");
	ArduinoOTA.setPassword("262626");  // простой пример пароля
	ArduinoOTA.begin();
	Serial.println("OTA Ready");
delay(100);
	Serial.println("OTA Ready");
	
}

void loop() {
	ArduinoOTA.handle();  // нужно вызывать регулярно
	                      //server.handleClient();
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	 if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // Читаем UID карты
      memset(uidBuffer, 0, sizeof(uidBuffer));
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        sprintf(uidBuffer + i * 2, "%02X", mfrc522.uid.uidByte[i]);
      }

      Serial.print("🆔 UID считан: ");
      Serial.println(uidBuffer);

      // mfrc522.PICC_HaltA();  // Остановить карту
      mfrc522.PCD_StopCrypto1();
	 }
}
