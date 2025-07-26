#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

const char* ssid = "LMT-5DA0";
const char* password = "IH7R9QCREC";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Настройка OTA
  ArduinoOTA.setHostname("esp12-ota"); // имя устройства
  ArduinoOTA.setPassword("262626"); // простой пример пароля
  ArduinoOTA.begin();

  Serial.println("Ready for OTA");
}

void loop() {
  ArduinoOTA.handle(); // обязательно вызывать в loop!
}