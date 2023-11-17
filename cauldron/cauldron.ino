#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

class Mp3Notify {
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action) {
    if (source & DfMp3_PlaySources_Sd) {
      //    Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) {
      //  Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) {
      //    Serial.print("Flash, ");
    }
    //  Serial.println(action);
  }
  static void OnError(uint16_t errorCode) {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track) {
    //Serial.print("Play finished for #");
    // Serial.println(track);
  }
  static void OnPlaySourceOnline(DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "removed");
  }
};

SoftwareSerial secondarySerial(7, 8);  // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);

#include "IRLremote.h"
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>  // библиотека "RFID".

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
String uidDec;     // для храниения номера метки в десятичном формате
String uidDecOld;  // для храниения номера метки в десятичном формате
Servo servo;

#include <IRremote.h>
int RECV_PIN = 3;
unsigned long irvalue;
IRrecv irrecv(RECV_PIN);
decode_results results;

#include <Adafruit_NeoPixel.h>

#define LED_PIN 9
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 11
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

long firstPixelHue;
uint32_t timerRainbow;
uint32_t timerSound;
uint32_t timerSmoke;
byte Smokecloc = 0;
byte rainbowstop = 0;
//potions
byte clocpationsl;  // счётчик совпадени. как только он равен размеру массива, значит это зелье сварено
byte first = 0;
byte clocfirst = 0;
byte startcoldren = 0;
const char* potions[][12] = {
  {
    //  0 Зелье Плавунчика или Морское зелье +
    "40D9EF2A96A80",  // 0 Чешуя рыбы убийцы
    "4FB9FF2A96A80",  // 1 Вытяжка зародыше апаллала
    "4AC9EF2A96A80",  // 2 Хребты Рыбы-Льва
    "49F9EF2A96A80",  // 3 Сок мурлакомля
    "4749FF2A96A81",  // 4 Стандартный ингридиент Н
    "4849FF2A96A81",  // 5 Морские жёлуди
    "4039FF2A96A81",  // 6 Безумные Многоножки
    "4A89EF2A96A80",  // 7 Лёд со дна серебристого озера
    "4FF9FF2A96A80",  // 8 Слизь Флоббер-Червя
    "x",              // 9
    "x",              // 10
    "x",              // 11

  },

  {
    //1 злодейское зелье
    "44F9DF2A96A80",  // 0 Плоть мертвеца
    "4B89EF2A96A80",  // 1 Ядовитый Колокольчик
    "43F9DF2A96A80",  // 2 Волосы Ламии
    "4319DF2A96A81",  // 3 Порошок из костей фестрала
    "46A9EF2A96A80",  // 4 Лоскут одежды Баньши
    "4639DF2A96A80",  // 5 икра красной Жабы-людоеда
    "4469EF2A96A81",  // 6 Сушёная крапива
    "47C9FF2A96A81",  // 7 Экстракт Фенхеля
    "48C9FF2A96A81",  // 8 Глаз Слепого Кота
    "4889FF2A96A81",  // 9 Крысиная Селезёнка
    "40B9FF2A96A81",  // 10 Кровь Саламандры
    "x",              // 11

  },
  {
    // 2 Зелье ловкости
    "4DE9EF2A96A80",  // 0 Жилы кентавра
    "4EF9FF2A96A80",  // 1 Когти грифона
    "45A9EF2A96A80",  // 2 Дыхание Амистра
    "4569EF2A96A81",  // 3 Иглы Дикобраза
    "4139FF2A96A81",  // 4 Крошечные глаза жуков
    "4749FF2A96A81",  // 4 Стандартный ингридиент Н
    "40E9DF2A96A80",  // 6 Костная Мука
    "4039FF2A96A81",  // 7 Безумные Многоножки
    "4199EF2A96A80",  // 8 Перья Выскакунчика
    "4B49EF2A96A80",  // 9 Огненная Соль
    "x",              // 10
    "x",              // 11

  },
  {
    // 3 Зелье «Каменная кожа»
    "4EE9EF2A96A80",  // 0 Шкура Саламандры
    "45E9EF2A96A81",  // 1 Драконья кровь
    "40E9DF2A96A80",  // 2 костная мука
    "4499DF2A96A81",  // 3 Прах вампира
    "4EF9FF2A96A80",  // 4 Когти грифона
    "4BF9FF2A96A80",  // 5 Порошок рога единорога
    "4119EF2A96A80",  // 6 Сок ягоды Бум
    "44E9EF2A96A81",  // 7 Капли мёртвой воды!!!!!!!!!!!11111111111111111111111111111111111111111111111
    "46E9EF2A96A80",  // 8 Сепосфера
    "4219EF2A96A80",  // 9 Собачий корень
    "4429EF2A96A81",  // 10 Корешки Маргариток
    "x",              // 11

  },
  {
    //4 Рвотное зелье
    "4079FF2A96A81",  // 0 Слизб болотника
    "4629EF2A96A81",  // 1 Сопли тролля
    "40A9DF2A96A80",  // 2 челюсти пучеглазки
    "43D9DF2A96A81",  // 3 Нашинкованные мёртвые гусенницы
    "4749FF2A96A81",  // 4 стандартный ингридиент Н
    "4849FF2A96A81",  // 5 Морские жёлуди
    "44B9DF2A96A80",  // 6 Травной стручёк
    "x",              // 7
    "x",              // 8
    "x",              // 9
    "x",              // 10
    "x",              // 11

  },

  {
    "4439DF2A96A80",  // 0
    "4219EF2A96A80",  // 1
    "40E9DF2A96A80",  // 2
    "40D9EF2A96A80",  // 3
    "4749FF2A96A81",  // 4
    "4099EF2A96A80",  // 5
    "x",              // 6
    "x",              // 7
    "x",              // 8
    "x",              // 9
    "x",              // 10
    "x",              // 11

  }


};

byte godlist[(sizeof(potions) / sizeof(potions[0]))];


void setup() {
  Serial.begin(115200);
  Serial.println("Waiting for card...");
  SPI.begin();         //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();  // инициализация MFRC522 / Init MFRC522 card.
  delay(500);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);  // turn the LED off by making the voltage LOW
  mp3.begin();
  mp3.setVolume(24);

  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)

  strip.setBrightness(210);  // Set BRIGHTNESS to about 1/5 (max = 255)

  irrecv.enableIRIn();  // Start the receiver
}
void loop() {

  if (startcoldren == 0) {

    if (irrecv.decode(&results)) {
      irvalue = results.value;
      irrecv.resume();  // Receive the next value
      Serial.println(irvalue);
      if ((irvalue == 16726215) || (irvalue == 1111000004)) {
        delay(100);
        mp3.playFolderTrack(3, 2);  // sd:/01/001.mp3
        delay(100);
        startcoldren = 1;
        Serial.println(irvalue);
        irvalue = "";
        servo.attach(6);   // attaches the servo on pin 9 to the servo object
        servo.write(178);  // sets the servo position according to the scaled value
        first = 0;

        if (irrecv.decode(&results)) {
          irvalue = results.value;
          irrecv.resume();  // Receive the next
        }
      }
    }

  } else if (startcoldren == 1) {
    if ((millis() - timerSound >= 300)) {  // ищем разницу (500 мс)
      timerSound = millis();               // сброс таймера
      if ((mp3.getStatus() == 512))        //&&(first!=0))
      {
        mp3.playFolderTrack(3, 2);  // sd:/01/001.mp3
      }
    }

    if ((millis() - timerRainbow >= 20) && (first == 0)) {
      timerRainbow = millis();  // сброс таймера
      rainbow();                // Flowing rainbow cycle along the whole strip
    }

    if (millis() - timerSmoke >= 2000) {
      Smokecloc++;
      timerSmoke = millis();  // сброс таймера
      Serial.println(Smokecloc);
      if (Smokecloc == 1) {
        digitalWrite(2, HIGH);  // turn the LED on (HIGH is the v  oltage level)
      } else if (Smokecloc == 40) {
        startcoldren = 0;
        Smokecloc = 0;
        mp3.stop();
        strip.clear();         // Set all pixel colors to 'off'
        strip.show();          // Send the updated pixel colors to the hardware.
        digitalWrite(2, LOW);  // turn the LED off by making the voltage LOW
        //  servo.detach();
      } else
        digitalWrite(2, LOW);  // turn the LED off by making the voltage LOW
    }

    // Поиск новой мет ки
    if (!mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    // Выбор метки
    if (!mfrc522.PICC_ReadCardSerial()) {
      return;
    }


    String content = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    content.toUpperCase();
    uidDec = content.substring(1);
    // Serial.print("UID : ");  //  "Сообщение: "
    // Serial.println(uidDec);  //  "Сообщение: "



    // есть массив пр колличеству зеле. при первом проходе туда заношу совпалдения по первому зелью и дальше обрабатываю зелья из него
    // как только по этому массиву нет совпадени не по одно строке (0) - значит не то зелье. Если есть полное совпажение по всем- сварили
    //findArray(uidDec);
    if (uidDecOld != uidDec) {
      if (first == 0) {
        // проходим по всем подряд. набиваем данные в godlist[]

        for (size_t i = 0; i < sizeof(potions) / sizeof(potions[0]); i++) {
          godlist[i] = uidDec == potions[i][0] ? 1 : 0;
          clocfirst += godlist[i] ? 1 : 0;

          /*
            Serial.print("clocfirst ");
            Serial.print(clocfirst);
            Serial.print(" i ");
            Serial.print(i);
            Serial.print(" Find ");
            Serial.println (godlist[i]);//  "Сообщение: "
          */
        }

        first = clocfirst > 0 ? 1 : 0;
        clocfirst = 0;

        if (first == 0) {
          mp3.stop();
          delay(200);
          mp3.playFolderTrack(3, 5);
          delay(100);
          uidDec = "";
          delay(3000);
        } else {
          mp3.stop();
          delay(200);
          mp3.playFolderTrack(3, 7);
          delay(100);
        }

        strip.clear();
        strip.show();
        delay(5);
        strip.setPixelColor(0, strip.Color(0, 150, 200));
        strip.show();  // Send the updated pixel colors to the hardware.
      }

      else {
        // next read uid
        Serial.println(" next read");

        for (size_t i = 0; i < sizeof(godlist); i++) {
          // Если вернулось 1 - значит по это строке (i) нужно проверить номер по first  с уид. И если совпало, ставим 1, если нет - 0;
          if (godlist[i] != 0) {

            if (godlist[i] == 4) Smokecloc = 0;

            if (uidDec == potions[i][godlist[i]]) {
              mp3.stop();
              delay(200);
              mp3.playFolderTrack(3, 7);
              delay(100);
              if (potions[i][godlist[i] + 1] == "x") {
                strip.setPixelColor(godlist[i], strip.Color(0, 150, 200));
                strip.show();  // Send the updated pixel colors to the hardware.
                first = 255;
                Serial.println("Open!!!");
                delay(2500);
                servo.detach();
                rainbowstop = 0;
                servo.attach(6);  // attaches the servo on pin 9 to the servo object
                servo.write(90);  // sets the servo position according to the scaled value
                delay(500);
                servo.detach();

                if (i == 0) {
                  servo.attach(A0);
                } else if (i == 1) {
                  servo.attach(A1);
                } else if (i == 2) {
                  servo.attach(A2);
                } else if (i == 3) {
                  servo.attach(A3);
                } else if (i == 4) {
                  servo.attach(4);
                }
                servo.write(100);
                delay(500);
                servo.detach();
                first = 0;
                startcoldren = 0;
                digitalWrite(2, LOW);  // turn the LED off by making the voltage LOW
                mp3.stop();
                strip.clear();  // Set all pixel colors to 'off'
              }
              strip.setPixelColor(godlist[i], strip.Color(0, 150, 200));
              strip.show();  // Send the updated pixel colors to the hardware.
              godlist[i] = godlist[i] + 1;
              clocfirst++;
            }
          }
        }

        first = clocfirst > 0 ? 1 : 0;
        clocfirst = 0;

        if (first == 0) {
          mp3.stop();
          delay(200);
          mp3.playFolderTrack(3, 5);
          delay(7000);
          Smokecloc = 0;
          startcoldren = 0;
          mp3.stop();
          strip.clear();         // Set all pixel colors to 'off'
          strip.show();          // Send the updated pixel colors to the hardware.
          digitalWrite(2, LOW);  // turn the LED off by making the voltage LOW
          servo.detach();
        }
      }
    }
    uidDecOld = uidDec;
  }
}
void rainbow() {

  // if (firstPixelHue < 5*65536)
  firstPixelHue += 256;

  for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...

    int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
  }
  strip.show();  // Update strip with new contents
}
