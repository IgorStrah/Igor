
#include <SPI.h>
#include <MFRC522.h> // библиотека "RFID".

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
String uidDec;  // для храниения номера метки в десятичном формате
//String uidDecOld;  // для храниения номера метки в десятичном формате

#include <Adafruit_NeoPixel.h>
#define LED_PIN    2
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 4
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
 String worlds_s;
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
49F42921F6F80 - привет
49642921F6F80 - Пока
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
byte first = 2; // 0 - firt tep/ 1 - step to find, /2 - wit to tart /3  - wait to IR start / 4 - darkening
byte step_t = 0;
byte step_l = 15;
byte clocfirst = 0;
byte startcoldren = 0;
byte onoff; // 1- on, 0 off

byte word_t[][13]  =
{
  { //  привет - вкючение и вкл светодиод
    31,5,19,21,12,1,4,1,0,0,0,0,0,            // 

  },
  { //  Пока - включение и вкл светодиоды
    31,5,19,21,12,1,4,1,0,0,0,0,0,            // 

  },
  
  { //  ядухмага
    31,5,19,21,12,1,4,1,0,0,0,0,0,            // 

  },

  { // тутживётмагия
    18,19,18,7,9,3,6,18,12,1,4,9,31,            // 

  },

  { //  бойся ведьмы
    2,14,9,17,31,3,6,5,28,12,27,0,            // 

  },

  { // птица живая 
    15,18,9,22,1,7,27,3,1,31,0,            // 

  },

  { //  некрономикон
    13,6,10,16,14,13,14,12,9,10,14,13,0            // 

  },



  { // духи воокруг
    5,19,21,9,3,14,10,16,19,4,0,0,

  }


};



void setup() {
  Serial.begin(115200);
  Serial.println("Waiting for card...");
  SPI.begin();  //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();     // инициализация MFRC522 / Init MFRC522 card.
  delay(500);
  randomSeed(analogRead(0));
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(70); // Set BRIGHTNESS to about 1/5 (max = 255)


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
      uidDec = content.substring(1);
      Serial.print("UID : ");  //  "Сообщение: "
      Serial.println(uidDec);  //  "Сообщение: "


      if (uidDec =="49642921F6F80")
      {
        onoff++;
        if (onoff>=10)
        {
          for(int i=0; i<G; i++)
          {
        G--;R--;B--;
        for(int i=0; i<LED_COUNT; i++) { // For each pixel...
        strip.setPixelColor(i, strip.Color(R, G, B));
        strip.show(); 
        delay(10);  
         }
          }
        first=2;
        }
      }
       else { onoff=0;}

      if  (first==2) // привет
      {
                    
      if (uidDec =="49F42921F6F80")
      {
           G+=4;;R+=4;B+=4;
        for(int i=0; i<LED_COUNT; i++) { // For each pixel...
        strip.setPixelColor(i, strip.Color(R, G, B));
        strip.show();   
         }
        if (B>=30)
        {first=0;}
      }
      }



if (first==0)
{
  first=1;
  clocfirst = random(1, 7);
}
strip.clear(); 

if (step_l>16)
{G=step_l;R=1;B=1;}
else if (first==1)
{G=30;R=30;B=30;}
else if (first==4)
{

  R!=0?R--:R=0;
  G!=0?G--:G=0;
  B!=0?B--:B=0;  
  G==0?first=2:first=4;
  }

    
  for(int i=0; i<LED_COUNT; i++) { // For each pixel...
  strip.setPixelColor(i, strip.Color(R, G, B));
  strip.show();   
   }

            Serial.print(" clocfirst    ");
            Serial.println( clocfirst );
            Serial.print("[step_t] ");
            Serial.print(step_t);
            Serial.print(" step_l ");
            Serial.println (step_l);//  "Сообщение: "
   
 /* 
            
  
            Serial.print("word_t[clocfirst][step_t]     ");
            Serial.println(word_t[clocfirst][step_t]);
            Serial.print("String Uidnew " );
            Serial.println (fainduid((word_t[clocfirst][step_t])));//  "Сообщение: "
          
            
 
            Serial.print("R");
            Serial.print(R);
            Serial.print(" G ");
            Serial.print(G);
            Serial.print(" B ");
            Serial.println (B);//  "Сообщение: "
    */          

       
         if (first==1) 
         { 
         uidDec == fainduid((word_t[clocfirst][step_t])) ? step_l=+step_l+7 : step_l=15;
        
        if (step_t==13)
        {
          clocfirst=0;
          first=4;
          step_t=0;
         }
        
       
        if (step_l>=90)
        {
          step_t++;
          step_l=15;
        }
        if  (0 == word_t[clocfirst][step_t])
        {
          clocfirst=0;
          first=4;
          step_t=0;
         
        }
        
    delay(15);
      }
}


String fainduid(byte char_word) {
  String uidDecOld;
 switch (char_word) {
    case 1:
      uidDecOld = "4B042921F6F80";
      break;
    case 2:
      uidDecOld = "4B942921F6F80";
      break;
    case 3:
      uidDecOld = "4C143921F6F80";
      break;
    case 4:
      uidDecOld = "40143921F6F81";
      break;
    case 5:
      uidDecOld = "45143921F6F81";
      break;
    case 6:
      uidDecOld = "44043921F6F81";
      break;
    case 7:
      uidDecOld = "42E43921F6F81";
      break;
    case 8:
      uidDecOld = "44943921F6F81";
      break;
    case 9:
      uidDecOld = "AC2AD15";
      break;
    case 10:
      uidDecOld = "ABD8315";
      break;
    case 11:
      uidDecOld = "AB85B15";
      break;
    case 12:
      uidDecOld = "AB33315";
      break;
    case 13:
      uidDecOld = "AAE0315";
      break;
    case 14:
      uidDecOld = "AA54515";
      break;
    case 15:
      uidDecOld = "AA01515";
      break;
    case 16:
      uidDecOld = "A9E7315";
      break;
    case 17:
      uidDecOld = "A048815";
      break;
    case 18:
      uidDecOld = "A06F715";
      break;
    case 19:
      uidDecOld = "A09A415";
      break;
    case 20:
      uidDecOld = "A327715";
      break;
    case 21:
      uidDecOld = "A368615";
      break;
    case 22:
      uidDecOld = "A3AA815";
      break;
    case 23:
      uidDecOld = "A3EFA15";
      break;
    case 24:
      uidDecOld = "A437215";
      break;
    case 25:
      uidDecOld = "A480915";
      break;
    case 26:
      uidDecOld = "A4CB015";
      break;
    case 27:
      uidDecOld = "A517015";
      break;
    case 28:
      uidDecOld = "A559015";
      break;
    case 29:
      uidDecOld = "A594915";
      break;
    case 30:
      uidDecOld = "A922715";
      break;
    case 31:
      uidDecOld = "A95B515";
      break;
    case 32:
      uidDecOld ="49642921F6F80";
      break;
    case 33:
      uidDecOld = "49F42921F6F80";
      break;
    case 34:
      uidDecOld = "4A742921F6F80";
      break;
    case 35:
      uidDecOld = "48D42921F6F80";
      break;
    case 36:
      uidDecOld ="A95B515";
      break;
    case 37:
      uidDecOld = "A95B515";
    break;
    
    
    return uidDecOld;
  }
}
