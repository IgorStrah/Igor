#include <SPI.h>            // Подключаем библиотеку SPI
#include <MFRC522.h>        // Подключаем библиотеку MFRC522

const int RST_PIN = 5;      // Вывод RST
const int SS_PIN = 10;      // Вывод SDA (SS)

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Создаём объект MFRC522

void setup() 
{
  
  Serial.begin(9600);        // Инициализация последовательного порта
  SPI.begin();               // Инициализация SPI
  mfrc522.PCD_Init();        // Инициализация считывателя RC522
}

void loop() 
{
  if (!mfrc522.PICC_IsNewCardPresent())   // Ожидание прикладывания RFID-метки
     { 
       return;                            // Выход, если не приложена новая карта
     }
  if (!mfrc522.PICC_ReadCardSerial())     // Считываем данные
     {
       return;                            // Выход, если невозможно считать серийный номер
     }


      uidDec = 0;
  // Выдача серийного номера метки.
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec * 256 + uidDecTemp;
  }
  Serial.println("Card UID: ");
  Serial.println(uidDec); // Выводим UID метки в консоль.


  
