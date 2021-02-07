
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
#define LED_PIN    8
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 4
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

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
byte clocfirst = 0;
byte startcoldren = 0;
const char  *word_t[][12]  =
{
  { //  0 Зелье Плавунчика или Морское зелье +
    "40D9EF2A96A80",     // 0 Чешуя рыбы убийцы
    "4FB9FF2A96A80",     // 1 Вытяжка зародыше апаллала
    "4AC9EF2A96A80",     // 2 Хребты Рыбы-Льва
    "49F9EF2A96A80",     // 3 Сок мурлакомля
    "4749FF2A96A81",     // 4 Стандартный ингридиент Н
    "4849FF2A96A81",     // 5 Морские жёлуди
    "4039FF2A96A81",     // 6 Безумные Многоножки
    "4A89EF2A96A80",     // 7 Лёд со дна серебристого озера
    "4FF9FF2A96A80",     // 8 Слизь Флоббер-Червя
    "x",                 // 9
    "x",                 // 10
    "x",                 // 11

  },

  { //1 злодейское зелье
    "44F9DF2A96A80",     // 0 Плоть мертвеца
    "4B89EF2A96A80",     // 1 Ядовитый Колокольчик
    "43F9DF2A96A80",     // 2 Волосы Ламии
    "4319DF2A96A81",     // 3 Порошок из костей фестрала
    "46A9EF2A96A80",     // 4 Лоскут одежды Баньши
    "4639DF2A96A80",     // 5 икра красной Жабы-людоеда
    "4469EF2A96A81",     // 6 Сушёная крапива
    "47C9FF2A96A81",     // 7 Экстракт Фенхеля
    "48C9FF2A96A81",     // 8 Глаз Слепого Кота
    "4889FF2A96A81",     // 9 Крысиная Селезёнка
    "40B9FF2A96A81",     // 10 Кровь Саламандры
    "x",                 // 11

  },
  { // 2 Зелье ловкости
    "4DE9EF2A96A80",     // 0 Жилы кентавра
    "4EF9FF2A96A80",     // 1 Когти грифона
    "45A9EF2A96A80",     // 2 Дыхание Амистра
    "4569EF2A96A81",     // 3 Иглы Дикобраза
    "4139FF2A96A81",     // 4 Крошечные глаза жуков
    "4749FF2A96A81",     // 4 Стандартный ингридиент Н
    "40E9DF2A96A80",     // 6 Костная Мука
    "4039FF2A96A81",     // 7 Безумные Многоножки
    "4199EF2A96A80",     // 8 Перья Выскакунчика
    "4B49EF2A96A80",     // 9 Огненная Соль
    "x",                 // 10
    "x",                 // 11

  },
  { // 3 Зелье «Каменная кожа»
    "4EE9EF2A96A80",   // 0 Шкура Саламандры
    "45E9EF2A96A81",     // 1 Драконья кровь
    "40E9DF2A96A80",     // 2 костная мука
    "4499DF2A96A81",     // 3 Прах вампира
    "4EF9FF2A96A80",     // 4 Когти грифона
    "4BF9FF2A96A80",     // 5 Порошок рога единорога
    "4119EF2A96A80",     // 6 Сок ягоды Бум
    "44E9EF2A96A81",     // 7 Капли мёртвой воды!!!!!!!!!!!11111111111111111111111111111111111111111111111
    "46E9EF2A96A80",     // 8 Сепосфера
    "4219EF2A96A80",     // 9 Собачий корень
    "4429EF2A96A81",     // 10 Корешки Маргариток
    "x",                 // 11

  },
  {
    //4 Рвотное зелье
    "4079FF2A96A81",     // 0 Слизб болотника
    "4629EF2A96A81",     // 1 Сопли тролля
    "40A9DF2A96A80",     // 2 челюсти пучеглазки
    "43D9DF2A96A81",     // 3 Нашинкованные мёртвые гусенницы
    "4749FF2A96A81",     // 4 стандартный ингридиент Н
    "4849FF2A96A81",     // 5 Морские жёлуди
    "44B9DF2A96A80",     // 6 Травной стручёк
    "x",                 // 7
    "x",                 // 8
    "x",                 // 9
    "x",                 // 10
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
  
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(210); // Set BRIGHTNESS to about 1/5 (max = 255)
  irrecv.enableIRIn(); // Start the receiver

}
void loop() {

  
    if (irrecv.decode(&results)) {
      irvalue = results.value;
      irrecv.resume(); // Receive the next value
      Serial.println(irvalue);
      if ((irvalue == 16726215) || (irvalue == 1111000004)) {
        delay (100);
       
        Serial.println(irvalue);
      if (irrecv.decode(&results)) {
      irvalue = results.value;
      irrecv.resume(); // Receive the next
        }
      }
    }

 

    if ((millis() - timerRainbow >= 20) && (first == 0))
    {
      timerRainbow = millis();              // сброс таймера
      rainbow();             // Flowing rainbow cycle along the whole strip
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



   
}



void rainbow() {

  // if (firstPixelHue < 5*65536)
  firstPixelHue += 256;

  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...

    int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
  }
  strip.show(); // Update strip with new contents

}
