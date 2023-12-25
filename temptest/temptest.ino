#include <OneWire.h>

OneWire ds(2); // Объект OneWire

int temperature = 0; // Глобальная переменная для хранения значение температуры с датчика DS18B20
int temperatureOld = 0;
int temperatureDelte = 0;
long lastUpdateTime = 0; // Переменная для хранения времени последнего считывания с датчика
const int TEMP_UPDATE_TIME = 1000; // Определяем периодичность проверок

void setup(){
  Serial.begin(115200);
}

void loop(){
  detectTemperature(); // Определяем температуру от датчика DS18b20
  Serial.println(temperature); // Выводим полученное значение температуры
    Serial.println( "" ); // Выводим полученное значение температуры
      Serial.println(temperature-temperatureOld); // Выводим полученное значение температуры
  // Т.к. переменная temperature имеет тип int, дробная часть будет просто отбрасываться
  temperatureOld=temperature;
  delay(1000);
}

int detectTemperature(){

  byte data[2];
  ds.reset();
  ds.write(0xCC);
  ds.write(0x44);

  if (millis() - lastUpdateTime > TEMP_UPDATE_TIME)
  {
    lastUpdateTime = millis();
    ds.reset();
    ds.write(0xCC);
    ds.write(0xBE);
    data[0] = ds.read();
    data[1] = ds.read();

    // Формируем значение
      float temperature1 =  ((data[1] << 8) | data[0]) * 0.0625;
    temperature =temperature1*100;// (data[1] << 8) + data[0]; temperature = temperature >> 4;
   // temperature=temperature;
  }
}