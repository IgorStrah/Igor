
#include "IRLremote.h"
#include <SPI.h>
#include <MFRC522.h> // библиотека "RFID".

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
String uidDec;  // для храниения номера метки в десятичном формате
String uidDecOld;  // для храниения номера метки в десятичном формате

#include <IRremote.h>
int RECV_PIN = 3;
unsigned long irvalue;
IRrecv irrecv(RECV_PIN);
decode_results results;
#include <Adafruit_NeoPixel.h>
#define LED_PIN    2
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 4
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

byte R,G,B;
long firstPixelHue ;
uint32_t timerRainbow;

byte rainbowstop = 0;
//potions

/*
4B042921F6F80 - A
4B942921F6F80 - Б
4C143921F6F80 - В
40143921F6F81 - Г
45143921F6F81 - Д
44043921F6F81 - Е
42E43921F6F81 - Ж
44943921F6F81 - З
AC2AD15 - И
ABD8315 - К
AB85B15 - Л
AB33315 - М
AAE0315 - Н
AA54515 - О
AA01515 - П
A9E7315 - Р
A048815 - С
A06F715 - Т
A09A415 - У
A327715 - Ф
A368615 - Х
A3AA815 - Ц
A3EFA15 - Ч
A437215 - Ш
A480915 - Щ
A4CB015 - Ъ
A517015 - Ы
A559015 - Ь
A594915 - Э
A922715 - Ю
A95B515 - Я
49642921F6F80 - привет
49F42921F6F80 - Пока
4A742921F6F80 - Нет
48D42921F6F80 - да
4A742921F6F80 - 0
41C43921F6F81 - 9
41343921F6F81 - 8
40A43921F6F81 - 7
4F843921F6F80 - 6
4EF43921F6F80 - 5 
4E643921F6F80 - 4
4DC43921F6F80 - 3 
4D343921F6F80 - 2
4CA43921F6F80 - 1


 */


byte clocpationsl;// счётчик совпадени. как только он равен размеру массива, значит это зелье сварено
byte first = 0;
byte step_t = 0;
byte step_l = 15;
byte clocfirst = 0;
byte startcoldren = 0;
const char  *word_t[][12]  =
{
  { //  0 Милая Скво
    "AB33315",      // 
    "AC2AD15",      // 
    "AB85B15",      // 
    "4B042921F6F80",// 
    "A95B515",      //
    "A048815",      // 
    "ABD8315",      // 
    "4C143921F6F80",// 
    "AA54515",      // 
    "x",            // 
    "x",            // 
    "x",            // 

  },

  
  {
    //1 духи рядом
    "45143921F6F81",     // 0 
    "A09A415",     // 1 
    "A368615",     // 2 
    "AC2AD15",     // 3 
    "A9E7315",     // 4 
    "A95B515",     // 5 
    "45143921F6F81",     // 6 
    "AA54515",                 // 7
    "AB33315",                 // 8
    "x",                 // 9
    "x",                 // 10
    "x",                 // 11

  },

  { //1 ищи в книгах
    "AC2AD15",     // 0
    "A480915",     // 1
    "AC2AD15",     // 2
    "4C143921F6F80",     // 3
    "ABD8315",     // 4
    "AAE0315",     // 5
    "AC2AD15",                 // 6
    "40143921F6F81",                 // 7
    "4B042921F6F80",                 // 8
    "A368615",                 // 9
    "x",                 // 10
    "x",                 // 11

  },
  {  //2 нужем медиум
    "AAE0315",     // 0
    "A09A415",     // 1
    "42E43921F6F81",     // 2
    "44043921F6F81",     // 3
    "AAE0315",     // 4
    "AB33315",     // 5
    "44043921F6F81",                 // 6
    "45143921F6F81",                 // 7
    "AC2AD15",                 // 8
    "A09A415",                 // 9
    "AB33315",                 // 10
    "x",                 // 11

  },
  {  //3 чую магию
    "A3EFA15",     // 0
    "A09A415",     // 1
    "A922715",     // 2
    "AB33315",     // 3
    "4B042921F6F80",     // 4
    "40143921F6F81",     // 5
    "AC2AD15",                 // 6
    "A922715",                 // 7
    "x",                 // 8
    "x",                 // 9
    "x",                 // 10
    "x",                 // 11

  },
  { // 4 птица в сети
    "AA01515",     // 0
    "A06F715",     // 1
    "AC2AD15",     // 2
    "A3AA815",     // 3
    "4B042921F6F80",//4
    "4C143921F6F80",     // 5
    "A048815",     // 6
    "44043921F6F81",           // 7
    "A06F715",                 // 8
    "AC2AD15",                 // 9
    "x",                 // 10
    "x",                 // 11
    

  },
  {  // 5 оживи ведьму
    "AA54515",     // 0
    "42E43921F6F81",     // 1
    "AC2AD15",     // 2
    "4C143921F6F80",     // 3
    "AC2AD15",     // 4
    "4C143921F6F80",     // 5
    "44043921F6F81",                 // 6
    "45143921F6F81",                 // 7
    "A559015",                 // 8
    "AB33315",                 // 9
    "A09A415",                 // 10
    "x",                 // 11

  },
  {
    "4439DF2A96A80",     // 0
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

  }


};

byte godlist[(sizeof(word_t) / sizeof(word_t[0]))];


void setup() {
  Serial.begin(115200);
  Serial.println("Waiting for card...");
  SPI.begin();  //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();     // инициализация MFRC522 / Init MFRC522 card.
  delay(500);
    randomSeed(analogRead(0));
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(70); // Set BRIGHTNESS to about 1/5 (max = 255)
  irrecv.enableIRIn(); // Start the receiver

}
void loop() {


if (first==0)
{
  first=1;
  clocfirst = random(1, 6);
}
strip.clear(); // 

if (step_l>16)
{G=step_l;R=1;B=1;}
else
{G=30;R=30;B=30;}
  
  for(int i=0; i<LED_COUNT; i++) { // For each pixel...
  strip.setPixelColor(i, strip.Color(R, G, B));
  strip.show();   
   }

  
    if (irrecv.decode(&results)) {
      irvalue = results.value;
      irrecv.resume(); // Receive the next value
      Serial.println(irvalue);
      if ((irvalue == 16726215) || (irvalue == 1111000006)) {
       delay (100);
       
        Serial.println(irvalue);
      if (irrecv.decode(&results)) {
      irvalue = results.value;
      irrecv.resume(); // Receive the next
        }
      }
    }

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
    uidDec = content.substring(1);
     Serial.print("UID : ");  //  "Сообщение: "
     Serial.println(uidDec);  //  "Сообщение: "


            Serial.print("clocfirst ");
            Serial.print(clocfirst);
            Serial.print("[step_t] ");
            Serial.print(step_t);
            Serial.print(" uidDec ");
            Serial.print(uidDec);
            Serial.print(" step_l ");
            Serial.println (step_l);//  "Сообщение: "

            Serial.print("R");
            Serial.print(R);
            Serial.print(" G ");
            Serial.print(G);
            Serial.print(" B ");
            Serial.println (B);//  "Сообщение: "
       
        uidDec == word_t[clocfirst][step_t] ? step_l=+step_l+5 : step_l=15;
        
        if (step_t==11)
        {
          clocfirst=0;
          first=0;
          step_t=0;
        }
        
        else if  ("x"== word_t[clocfirst][step_t+1])
        {
          clocfirst=0;
          first=0;
          step_t=0;
        }
        if (step_l>=90)
        {
          step_t++;
          step_l=15;
        }
        
          /*
            Serial.print("clocfirst ");
            Serial.print(clocfirst);
            Serial.print(" i ");
            Serial.print(i);
            Serial.print(" Find ");
            Serial.println (godlist[i]);//  "Сообщение: "
          */
delay(20);

      }
    
  
