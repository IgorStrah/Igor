#include <SoftwareSerial.h>
#include <DFMiniMp3.h>


class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
    //    Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
      //  Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
    //    Serial.print("Flash, ");
    }
  //  Serial.println(action);
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
    //Serial.print("Play finished for #");
   // Serial.println(track);  
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
String uidDec;  // для храниения номера метки в десятичном формате
String uidDecOld;  // для храниения номера метки в десятичном формате
Servo servo;

#include <IRremote.h>
int RECV_PIN = 3;
unsigned long irvalue;
IRrecv irrecv(RECV_PIN);
decode_results results;

#include <Adafruit_NeoPixel.h>


#define LED_PIN    9
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 11
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

long firstPixelHue ;
uint32_t timerRainbow;
uint32_t timerSound;
uint32_t timerSmoke;
byte Smokecloc=0;
byte rainbowstop=0;
//potions
int clocpationsl;// счётчик совпадени. как только он равен размеру массива, значит это зелье сварено
byte first=0;
byte clocfirst=0;
byte startcoldren=0;
const char *potions[][12]  = 
{
{
  "4439DF2A96A80",   // 0
  "4219EF2A96A80",     // 1
  "40E9DF2A96A80",     // 2
  "40D9EF2A96A80",     // 3
  "4749FF2A96A81",     // 4
  "4099EF2A96A80",     // 5
  "x",                 // 6
  "x",                 // 7
  "x",                 // 8
  "x",                 // 9
  "x",                 // 10
  "x",                 // 11
  
},

{
  "4439DF2A96A80",   // 0
  "4219EF2A96A80",     // 1
  "40E9DF2A96A80",     // 2
  "40D9EF2A96A80",     // 3
  "4749FF2A96A81",     // 4
  "4099EF2A96A80",     // 5
  "x",                 // 6
  "x",                 // 7
  "x",                 // 8
  "x",                 // 9
  "x",                 // 10
  "x",                 // 11
  
},
{
  "4439DF2A96A80",   // 0
  "4219EF2A96A80",     // 1
  "40E9DF2A96A80",     // 2
  "40D9EF2A96A80",     // 3
  "4749FF2A96A81",     // 4
  "4099EF2A96A80",     // 5
  "x",                 // 6
  "x",                 // 7
  "x",                 // 8
  "x",                 // 9
  "x",                 // 10
  "x",                 // 11
  
},
{
  "4219EF2A96A80",   // 0
  "4219EF2A96A80",     // 1
  "40E9DF2A96A80",     // 2
  "40D9EF2A96A80",     // 3
  "4749FF2A96A81",     // 4
  "4099EF2A96A80",     // 5
  "x",                 // 6
  "x",                 // 7
  "x",                 // 8
  "x",                 // 9
  "x",                 // 10
  "x",                 // 11
  
},
{
  
  "4439DF2A96A80",   // 0
  "4219EF2A96A80",     // 1
  "40E9DF2A96A80",     // 2
  "40D9EF2A96A80",     // 3
  "4749FF2A96A81",     // 4
  "4099EF2A96A80",     // 5
  "x",                 // 6
  "x",                 // 7
  "x",                 // 8
  "x",                 // 9
  "x",                 // 10
  "x",                 // 11
  
},
{
  "4439DF2A96A80",   // 0
  "4219EF2A96A80",     // 1
  "40E9DF2A96A80",     // 2
  "40D9EF2A96A80",     // 3
  "4749FF2A96A81",     // 4
  "4099EF2A96A80",     // 5
  "x",                 // 6
  "x",                 // 7
  "x",                 // 8
  "x",                 // 9
  "x",                 // 10
  "x",                 // 11
  
},
{
  "4219EF2A96A80",   // 0
  "4439DF2A96A80",     // 1
  "40D9EF2A96A80",     // 2
  "4749FF2A96A81",     // 3
  "4099EF2A96A80",     // 4
  "40E9DF2A96A80",     // 5
  "4FA9EF2A96A80",     // 6
  "4709FF2A96A81",     // 7
  "4199EF2A96A80",     // 8
  "4C39FF2A96A80",     // 9
  "49F9EF2A96A80",     // 10
  "x",                 // 11
}


  
};



byte godlist[(sizeof(potions) / sizeof(potions[0]))];







void setup() {
  Serial.begin(115200);
  Serial.println("Waiting for card...");
  SPI.begin();  //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();     // инициализация MFRC522 / Init MFRC522 card.
  delay(500);
  pinMode(2, OUTPUT);

  mp3.begin();
  uint16_t volume = mp3.getVolume();
  Serial.print("volume was ");
  Serial.println(volume);
  mp3.setVolume(20);
  volume = mp3.getVolume();
  Serial.print(" and changed to  ");
  Serial.println(volume);

 // strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
 // strip.show();            // Turn OFF all pixels ASAP
 // strip.setBrightness(210); // Set BRIGHTNESS to about 1/5 (max = 255)
  Serial.println(" sizeof(godlist) ");
  Serial.println(sizeof(godlist));

 
 irrecv.enableIRIn(); // Start the receiver
 servo.attach(6);  // attaches the servo on pin 9 to the servo object
 servo.write(164);                  // sets the servo position according to the scaled value

  //myservo.write(60);                  // sets the servo position according to the scaled value

}
void loop() {


  // Поиск новой мет ки
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Выбор метки
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }


  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();
  uidDec=content.substring(1);
  // Serial.print("UID : ");  //  "Сообщение: "
 // Serial.println(uidDec);  //  "Сообщение: "



// есть массив пр колличеству зеле. при первом проходе туда заношу совпалдения по первому зелью и дальше обрабатываю зелья из него
// как только по этому массиву нет совпадени не по одно строке (0) - значит не то зелье. Если есть полное совпажение по всем- сварили 
//findArray(uidDec);
if (uidDecOld!=uidDec)
{
  Serial.print("UID : ");  //  "Сообщение: "
  Serial.println(uidDec);  //  "Сообщение: "
   delay(1000);
   }


}
void rainbow() {
 
 // if (firstPixelHue < 5*65536)
   firstPixelHue += 256;
    
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    
  }





  void findArray(String uid) {
    
  
    
 
  }
