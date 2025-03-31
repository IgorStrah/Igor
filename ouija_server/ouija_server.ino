#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 8);  // CE, CSN
const byte serverAddress[6] = "Server";
const byte clientAddress[6] = "Client";

char receivedUID[16];
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 5000; // Отправка каждые 5 секунд

void setup() {
    Serial.begin(115200);
    
    radio.begin();
    radio.setPALevel(RF24_PA_HIGH);
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(5, 15);
    radio.setAutoAck(true);
    
    radio.openWritingPipe(clientAddress);
    radio.openReadingPipe(1, serverAddress);
    radio.startListening();

    Serial.println("📡 Сервер RFID + nRF24L01 запущен...");
}

void loop() {
    if (radio.available()) {
        radio.read(&receivedUID, sizeof(receivedUID));
        Serial.print("📩 UID получен от клиента: ");
        Serial.println(receivedUID);
    }

 if (millis() - lastSendTime >= sendInterval) {
        struct {
            int vib;
            int timevib;
        } data = {20, 300};  // Пример значений вибрации и времени
 radio.stopListening();
        Serial.println("Sending data...");
        bool success = radio.write(&data, sizeof(data));

        if (success) {
            Serial.println("Data sent successfully");
        } else {
            Serial.println("Failed to send data");
        }
          radio.startListening();
        lastSendTime = millis();
    }


}
