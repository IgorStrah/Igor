#include <RF24.h>

// Создаём объект radio для работы с nRF24L01+
RF24 radio(9, 10);

// Адреса труб
byte address[][6] = {
  "1Node",  // Адрес для трубы 1 (слушаем сервер, если нужно)
  "2Node",  // Адрес для трубы 2 (слушаем данные от сервера)
};

void setup() {
  Serial.begin(115200);
  radio.begin();
   radio.setAutoAck(1);                   // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);               // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();              // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);              // размер пакета, байт
  radio.setPALevel(RF24_PA_HIGH);  // Устанавливаем мощность передачи
 radio.setChannel(0x95);  
  // Открываем трубы
  radio.openReadingPipe(1, address[1]);  // Чтение с трубы 1 (если сервер отправляет на трубу 1)
  radio.openReadingPipe(2, address[21]);  // Чтение с трубы 2 (получаем данные от сервера)
  radio.powerUp();         // начать работу
  radio.startListening();  // начинаем слушать эфир, мы приёмный модуль
}

void loop() {
  // Проверка, если доступны данные от сервера (на трубе 2)
  if (radio.available()) {
    char receivedData[32];
    radio.read(&receivedData, sizeof(receivedData));  // Чтение данных от сервера

    // Обработка полученных данных от сервера
    Serial.print("Received from server on device 2: ");
    Serial.println(receivedData);
  }
}
