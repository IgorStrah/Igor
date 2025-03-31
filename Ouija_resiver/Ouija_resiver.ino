#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 8);  // CE, CSN
const byte serverAddress[6] = "Server";
const byte clientAddress[6] = "Client";

byte number = 0;

void setup() {
    Serial.begin(115200); Serial.println("📡 Сервер запущен...");
    radio.begin();
    radio.setPALevel(RF24_PA_HIGH);
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(5, 15);
    radio.setAutoAck(true);
    
    radio.openWritingPipe(clientAddress);  // Сервер отправляет клиенту
    radio.openReadingPipe(1, serverAddress);  // Сервер слушает
    radio.startListening();

   
}

void loop() {
    // Отправка числа клиенту
    radio.stopListening();
    radio.flush_tx();  // Очищаем буфер отправки

    if (radio.write(&number, sizeof(number))) {
        Serial.print("📤 Отправлено клиенту: ");
        Serial.println(number);
    } else {
        Serial.println("❌ Ошибка отправки!");
    }

    radio.startListening();
    radio.flush_rx();  // Очищаем буфер приёма

    unsigned long startTime = millis();
    while (!radio.available() && millis() - startTime < 200) {
        // Ждём ответ 200 мс
    }

    if (radio.available()) {
        radio.read(&number, sizeof(number));
        Serial.print("📩 Получено от клиента: ");
        Serial.println(number);
    } else {
        Serial.println("❌ Ответ от клиента не получен!");
    }

    number++;  // Увеличиваем число
    if (number > 255) number = 0;

    delay(1000);  // Пауза 1 секунда
}
