#pragma once
#include <ESP8266WebServer.h>

void setupWebServer(ESP8266WebServer& server, bool& wifiConnected, String phrases[], int uidCount, String& debugLog);
void handleWebLoop();  // вызывать в loop(), если нужно
