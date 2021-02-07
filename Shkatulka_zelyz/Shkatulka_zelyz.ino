#include "IRLremote.h"
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h> // библиотека "RFID".



#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
SoftwareSerial mySerial(3, 5); // RX, TX
DFPlayerMini_Fast myMP3;
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long uidDec, uidDecTemp;  // для храниения номера метки в десятичном формате
Servo servo;

#include <IRremote.h>
int RECV_PIN = 8;
unsigned long irvalue;
IRrecv irrecv(RECV_PIN);

decode_results results;

void setup() {
  Serial.begin(115200);
  Serial.println("Waiting for card...");
  SPI.begin();  //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();     // инициализация MFRC522 / Init MFRC522 card.
  delay(5000);
  servo.attach(7);
  servo.write(7);  // устанавливаем серву в закрытое сосотояние

  mySerial.begin(9600);
  myMP3.begin(mySerial);

  delay (100);
  myMP3.volume(29);
  delay (100);

  irrecv.enableIRIn(); // Start the receiver

}
void loop() {


  if (irrecv.decode(&results)) {

    irvalue = results.value;
    irrecv.resume(); // Receive the next value
    Serial.println(irvalue);
    if (irvalue == 3772810873) {
      servo.write(7);  // устанавливаем серву в закрытое сосотояние
      delay(200);
      Serial.println(irvalue);
      irvalue = "";
    }
    if (irvalue == 1100000004) {
      delay (100);
      myMP3.play(4);
      delay (100);
      servo.write(160); // Поворациваем серву на угол 90 градусов(Отпираем какой либо механизм: задвижку, поворациваем ключ и т.д.)
      Serial.println(irvalue);
      irvalue = "";
      delay(15000); // пауза 3 сек и механизм запирается.
    }

    if (irvalue == 1100000002) {
      servo.write(160); // Поворациваем серву на угол 90 градусов(Отпираем какой либо механизм: задвижку, поворациваем ключ и т.д.)
      Serial.println(irvalue);
      irvalue = "";
      delay(1000); // пауза 3 сек и механизм запирается.
    }

    if (irvalue == 1100000003) {
      servo.write(7);  // устанавливаем серву в закрытое сосотояние
      delay(200);
      Serial.println(irvalue);
      irvalue = "";
    }




  }
  delay(100);


  // Поиск новой мет ки
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Выбор метки
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
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
  if (uidDec == 1776196974) // Сравниваем Uid метки, если он равен заданому то серва открывает.
  {

    delay (100);
    myMP3.play(4);
    delay (100);
    servo.write(160); // Поворациваем серву на угол 90 градусов(Отпираем какой либо механизм: задвижку, поворациваем ключ и т.д.)
    delay(15000); // пауза 3 сек и механизм запирается.

  }


}
