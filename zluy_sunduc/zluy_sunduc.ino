#include "IRLremote.h"
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h> // библиотека "RFID".

#include <CapacitiveSensor.h>
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4, 2);
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>


class Mp3Notify
{
  public:
    static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
    {
      if (source & DfMp3_PlaySources_Sd)
      {
        Serial.print("SD Card, ");
      }
      if (source & DfMp3_PlaySources_Usb)
      {
        Serial.print("USB Disk, ");
      }
      if (source & DfMp3_PlaySources_Flash)
      {
        Serial.print("Flash, ");
      }
      Serial.println(action);
    }
    static void OnError(uint16_t errorCode)
    {
      // see DfMp3_Error for code meaning
      Serial.println();
      Serial.print("Com Error ");
      Serial.println(errorCode);
    }
    static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
    {
      Serial.print("Play finished for #");
      Serial.println(track);
    }
    static void OnPlaySourceOnline(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "online");
    }
    static void OnPlaySourceInserted(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "inserted");
    }
    static void OnPlaySourceRemoved(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "removed");
    }
};


SoftwareSerial secondarySerial(3, 5); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);


#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
String uidDec, uidDecOld;  // для храниения номера метки в десятичном формате
Servo servo;

#include <IRremote.h>
int RECV_PIN = 8;
int openclose;
int stepread = 0;
byte startread=0;
unsigned long my_timer;
unsigned long irvalue;
IRrecv irrecv(RECV_PIN);

decode_results results;

byte readis;

void setup() {
  Serial.begin(115200);
  Serial.println("Waiting for card...");
  SPI.begin();  //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();     // инициализация MFRC522 / Init MFRC522 card.
  delay(5000);
  servo.attach(7);
  servo.write(7);  // устанавливаем серву в закрытое сосотояние
  openclose = 1;

  mp3.begin();
  mp3.setVolume(27);

  delay (100);
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
  irrecv.enableIRIn(); // Start the receiver
  my_timer = millis();


}
void loop() {
  long total1 =  cs_4_2.capacitiveSensor(30);
  
  if ((total1 > 35)&&(stepread!=7)) {
    mp3.playFolderTrack(1, 5); // sd:/01/001.mp3
    delay (2000);
  }

if (millis() - my_timer >= 90000) {
    my_timer = millis();   // "сбросить" таймер
    // дейтвия, которые хотим выполнить один раз за период
    if (startread==1)
    {
    startread=0;
    mp3.playFolderTrack(1, 12); //
    }
  }
  
  if (irrecv.decode(&results)) {

    
    irvalue = results.value;
    irrecv.resume(); // Receive the next value
    Serial.println(irvalue);

    if ((irvalue == 1111000004) || (irvalue == 16726215)) { // атака проведена верно  - сворачиваемся.
      delay (100);
      mp3.playFolderTrack(1, 13); //
      delay (100);
      //servo.write(160); // Поворациваем серву на угол 90 градусов(Отпираем какой либо механизм: задвижку, поворациваем ключ и т.д.)
      Serial.println(irvalue);
      irvalue = "";
      delay(150); // пауза 3 сек и механизм запирается.
      startread=1;
        Serial.println("Read");
    }

  if ((irvalue == 1111000006) || (irvalue == 16718055)) 
    {
      servo.write(7);  // устанавливаем серву в закрытое сосотояние
      delay(200);
      Serial.println(irvalue);
      irvalue = "";
    }
  }
  delay(100);


if (startread==1)
{
 Serial.println("started");

  // ищем новые карты:
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // выбираем одну из карт:

  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  // показываем UID на мониторе порта:
  Serial.print("UID tag :");  //  "UID тега: "
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();
  uidDec=content.substring(1);
   Serial.print("UID : ");  //  "Сообщение: "
   Serial.println(uidDec);  //  "Сообщение: "



// есть массив пр колличеству зеле. при первом проходе туда заношу совпалдения по первому зелью и дальше обрабатываю зелья из него
// как только по этому массиву нет совпадени не по одно строке (0) - значит не то зелье. Если есть полное совпажение по всем- сварили 
//findArray(uidDec);

if (uidDecOld!=uidDec)
{

// контейнер с зельями. потом вынесу в процедуру

  if (content.substring(1) == "9DE9D6E") 
  {
    content = "";
    stepread = 7;
    Serial.println("Authorized access");  //  IT
    Serial.print("stepread ");
    Serial.println(stepread);
    Serial.print("String ");
    Serial.println(content.substring(1) );
    delay (100);
    mp3.playFolderTrack(1, 11); //
    delay (1500);
    servo.write(160); // Поворациваем серву на угол 90 градусов(Отпираем какой либо механизм: задвижку, поворациваем ключ и т.д.)
    delay(500);
    servo.detach();
    startread=0;
  }
  else 
  {
    stepread = 0;
    delay (100);
    mp3.playFolderTrack(1, 12); //
    delay (100);
    // servo.write(160); // Поворациваем серву на угол 90 градусов(Отпираем какой либо механизм: задвижку, поворациваем ключ и т.д.)
  }

uidDecOld==uidDec;

}
}
}
