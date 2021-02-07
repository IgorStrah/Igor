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


SoftwareSerial secondarySerial(7, 8); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);


#include "IRLremote.h"
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h> // библиотека "RFID".





#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long uidDec, uidDecTemp;  // для храниения номера метки в десятичном формате
Servo servo;

#include <IRremote.h>
int RECV_PIN = 5;
unsigned long irvalue;
IRrecv irrecv(RECV_PIN);
decode_results results;

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN    9
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 11
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  Serial.println("Waiting for card...");
  SPI.begin();  //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();     // инициализация MFRC522 / Init MFRC522 card.
  delay(500);


  mp3.begin();
  uint16_t volume = mp3.getVolume();
  Serial.print("volume was ");
  Serial.println(volume);
  mp3.setVolume(20);
  volume = mp3.getVolume();
  Serial.print(" and changed to  ");
  Serial.println(volume);

   strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(210); // Set BRIGHTNESS to about 1/5 (max = 255)
  
 
 irrecv.enableIRIn(); // Start the receiver
 servo.attach(6);  // attaches the servo on pin 9 to the servo object
 servo.write(165);                  // sets the servo position according to the scaled value

  //myservo.write(60);                  // sets the servo position according to the scaled value

}
void loop() {
 

  if (irrecv.decode(&results)) {

    irvalue = results.value;
    irrecv.resume(); // Receive the next value
    Serial.println(irvalue);
    
    if (irvalue == 1100000001) {
    delay (100);
    mp3.playFolderTrack(2, 2); // sd:/01/001.mp3
    delay (100);
    //servo.write(160); // Поворациваем серву на угол 90 градусов(Отпираем какой либо механизм: задвижку, поворациваем ключ и т.д.)
    Serial.println(irvalue);
    irvalue = "";
    delay(15000); // пауза 3 сек и механизм запирается.
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
     mp3.playFolderTrack(2, 2); // sd:/01/001.mp3
    delay (100);
    servo.write(160); // Поворациваем серву на угол 90 градусов(Отпираем какой либо механизм: задвижку, поворациваем ключ и т.д.)
    delay(15000); // пауза 3 сек и механизм запирается.

  }


}
