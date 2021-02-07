#include "GyverBus.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif


#define TX_PIN 2    // пин
#define TX_ADDR 1   // наш адрес
#define RX_ADDR1 2   // адрес приёмника
#define NUMPIXELS 8 // Popular NeoPixel ring size
#define PIN        A1  // On Trinket or Gemma, suggest changing this to 1
// пин 4, адрес 1, буфер на 15 байт
GBus<TX_PIN, GBUS_FULL> tx(TX_ADDR, 15);

#include <LiquidCrystal.h>
#include <IRremote.h>

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

// instance a DFMiniMp3 object,
// defined with the above notification class and the hardware serial class
//
//DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial1);

// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definition and uncomment these lines
SoftwareSerial secondarySerial(A4, A5); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);




LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define BTN_UP   1
#define BTN_DOWN 2
#define BTN_LEFT 3
#define BTN_RIGHT 4
#define BTN_SELECT 5
#define BTN_NONE 10
int RECV_PIN = A3;
IRrecv irrecv(RECV_PIN);
decode_results results;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

byte Flstart = 0;
int sec = 0;
int clocsec = 0;
int pixnum = 0;
byte timer = 0;
long timerraibow = 0;
byte menupos = 0;
byte menupos2 = 0;
byte playmp3 = 0;
int KillMonstr = 0;
int button;
int val;
int transmitted = 0; // 0 wait/ 1 request/ 2 send
int transmittedclk = 0;
int RX_ADDR;    // адрес приёмника
int RX_ADDRold;
int error = 0;
int wait_rainbow;
long firstPixelHue;
int detectButton() {
  int keyAnalog =  analogRead(A0);
  if (keyAnalog < 90) {
    // Значение меньше 100 – нажата кнопка right
    return BTN_RIGHT;
  } else if (keyAnalog < 200) {
    // Значение больше 100 (иначе мы бы вошли в предыдущий блок результата сравнения, но меньше 200 – нажата кнопка UP
    return BTN_UP;
  } else if (keyAnalog < 400) {
    // Значение больше 200, но меньше 400 – нажата кнопка DOWN
    return BTN_DOWN;
  } else if (keyAnalog < 600) {
    // Значение больше 400, но меньше 600 – нажата кнопка LEFT
    return BTN_LEFT;
  } else if (keyAnalog < 800) {
    // Значение больше 600, но меньше 800 – нажата кнопка SELECT
    return BTN_SELECT;
  } else {
    // Все остальные значения (до 1023) будут означать, что нажатий не было
    return BTN_NONE;
  }
}


void clearLine(int line) {
  lcd.setCursor(0, 1);
  lcd.print("                ");
}
void printDisplay(String message) {
  Serial.println(message);
  lcd.setCursor(0, 1);
  lcd.print(message);
  delay(1000);
  clearLine(1);

}
void setup() {

  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); lcd.print(">Arcade  Seting");
  irrecv.enableIRIn(); // Start the receiver

  randomSeed(analogRead(A2));
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(254); // Set BRIGHTNESS to about 1/5 (max = 255)

  mp3.begin();
  mp3.setVolume(10);


}
void loop() {
  int button = detectButton();
  int nDevices = 0;
  menu(button);


  if (menupos == 0)
  {
    if (millis() - timerraibow >= 30) {
      timerraibow = millis();
      rainbow();
    }
  }


  if (irrecv.decode(&results)) {

    unsigned long irvalue;
    irvalue = results.value;
    irrecv.resume(); // Receive the next value
    Serial.println(irvalue);

    if ((irvalue == 1111000006) || (irvalue == 16718055)) { // атака проведена верно  - сворачиваемся.

      if (menupos != 1)
      {
        menupos = 1; transmitted = 0;  sec = 15;
        delay (100);
      }
    }
  }


  if ((menupos == 1) && (transmitted == 0))
  {
    // так я получил уникальный адрес рандомом, который не повторяется ( рандом такой рандом )
    RX_ADDRold = random(2, 10);
    while (RX_ADDRold == RX_ADDR)
    {
      RX_ADDRold = random(2, 10);
    }

    RX_ADDR = RX_ADDRold;
    transmittedclk = 50;

    clocsec++;
    if (clocsec == 5)
    {
      clocsec = 0;
      sec >= 4 ? sec-- : sec = 4;
    }

    transmitted = 2;
    tx.sendData(RX_ADDR, sec);


    Serial.print("RX_ADDR2 ");
    Serial.println(RX_ADDR);

    Serial.print("sec  ");
    Serial.println(sec);
  }


  if ((tx.tick() == TX_COMPLETE) && (transmitted == 2))
  {
    transmitted = 1;
    transmittedclk = 50;
    delay(100);
  }

  // если нам что-то отправили
  if (error == 3)
  {
    error = 0;
    transmitted = 0;
  }

  if (RX_ADDR == 2)
  {
    pixnum = 7;
  }
  else if (RX_ADDR == 3)
    pixnum = 5;
  else if (RX_ADDR == 4)
    pixnum = 6;
  else if (RX_ADDR == 5)
    pixnum = 1;
  else if (RX_ADDR == 6)
    pixnum = 4;
  else if (RX_ADDR == 7)
    pixnum = 2;
  else if (RX_ADDR == 8)
    pixnum = 0;
  else if (RX_ADDR == 9)
    pixnum = 3;


  static uint32_t tmr;
  if (millis() - tmr > 100) {
    tmr = millis();
    transmittedclk--;
    if (menupos != 0)
    {
      pixels.setPixelColor(pixnum, pixels.Color(0, 250, 250));
      pixels.show();
    }

    if (transmittedclk < 0)

    {
      transmitted = 0;
    }

    if (transmitted == 1)
    {

      pixels.clear();
      byte state = tx.sendRequestAck(RX_ADDR, 1, 700);
      state = tx.sendRequestAck(RX_ADDR, 3, 700);

      if (RX_ADDR == 2)
      {

        if (playmp3 == 0)
        {
          mp3.setVolume(25);
          playmp3 = 1;
          delay(50);
          mp3.stop();
          mp3.playFolderTrack(1, 1); // sd:/01/001.mp3

        }
      }


      if (RX_ADDR == 4)
      {

        if (playmp3 == 0)
        {
          mp3.setVolume(18);
          playmp3 = 1;
          delay(50);
          mp3.stop();
          mp3.playFolderTrack(1, 4); // sd:/01/001.mp3

        }
      }


      if (RX_ADDR == 9)
      {

        if (playmp3 == 0)
        {
          mp3.setVolume(20);
          playmp3 = 1;
          delay(50);
          mp3.stop();
          mp3.playFolderTrack(1, 6); // sd:/01/001.mp3

        }
      }


      if (RX_ADDR == 6)
      {

        if (playmp3 == 0)
        {
          mp3.setVolume(20);
          playmp3 = 1;
          delay(50);
          mp3.stop();
          mp3.playFolderTrack(1, 5); // sd:/01/001.mp3

        }
      }
      if (RX_ADDR == 6)
      {

        if (playmp3 == 0)
        {
          mp3.setVolume(20);
          playmp3 = 1;
          delay(50);
          mp3.stop();
          mp3.playFolderTrack(1, 8); // sd:/01/001.mp3

        }
      }

      if (RX_ADDR == 7)
      {

        if (playmp3 == 0)
        {
          mp3.setVolume(20);
          playmp3 = 1;
          delay(50);
          mp3.stop();
          mp3.playFolderTrack(1, 7); // sd:/01/001.mp3

        }
      }
      if (RX_ADDR == 8)
      {

        if (playmp3 == 0)
        {
          mp3.setVolume(20);
          playmp3 = 1;
          delay(50);
          mp3.stop();
          mp3.playFolderTrack(1, 7); // sd:/01/001.mp3

        }
      }

      switch (state) {
        case ACK_ERROR: Serial.println("ack error");
          error++;
          break;
        case ACK_ONLY: Serial.println("got ack");
          break;
        case ACK_DATA: Serial.println("got data: ");
          int val;
          tx.readData(val);
          transmittedclk++;


          Serial.print("RX_ADDR2 ");
          Serial.println(RX_ADDR);
          Serial.print("data  ");
          Serial.println(val);

          if ((val == 0) || (val == 1) )
          {
            pixels.setPixelColor(pixnum, pixels.Color(255, 0, 0));
            pixels.show();
            delay(100);
            menupos = 0;
            transmitted = 0;
            KillMonstr = 0;
            playmp3 = 0;
            lcd.setCursor(12, 1);
            lcd.print("   ");
            lcd.setCursor(13, 1);
            lcd.print("X_X");
            playmp3 = 0;
            if (playmp3 == 0)
            {
              playmp3 = 1;
              mp3.setVolume(8);
              delay(50);
              mp3.stop();
              mp3.playFolderTrack(1, 99); // sd:/01/001.mp3

            }

            Serial.print("dedth  ");
            Serial.println(val);
          }

          else if ((val >= 20) && (val <= 95) )
          { transmitted = 2;

          }

          else if  ((val == 99) || (val == 98))
          {
            playmp3 = 0;
            if (playmp3 == 0)
            {
              playmp3 = 1;
              mp3.setVolume(15);
              delay(50);
              mp3.stop();
              mp3.playFolderTrack(1, 3); // sd:/01/001.mp3

            }

            pixels.setPixelColor(pixnum, pixels.Color(255, 255, 0));
            pixels.show();
            transmitted = 0;
            KillMonstr++;

            playmp3 = 0;
            lcd.setCursor(12, 1);
            lcd.print("  ");
            lcd.setCursor(13, 1);
            lcd.print("Kill");


            lcd.setCursor(0, 0);
            lcd.print("                ");
            lcd.setCursor(6, 0);
            lcd.print("Monstrs");
            lcd.setCursor(14, 0);
            lcd.print(KillMonstr);

          }

          break;
      }
    }
  }
}



void menu( int key)
{


  if (menupos == 0)

  {

    if (key == BTN_RIGHT)
    {
      lcd.setCursor(0, 0); lcd.print(" Arcade >Seting");
      delay(100);
      menupos2 = 1;
    }

    if (key == BTN_LEFT)
    { lcd.setCursor(0, 0); lcd.print(">Arcade  Seting");
      delay(100);
      menupos2 = 0;
    }

    if (key == BTN_SELECT)
    {
      if (menupos2 == 0)
      {
        menupos = 1;
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("Start  Arcade");
        lcd.noBlink();
        sec = 15;
        lcd.setCursor(11, 1);
        lcd.print("Sec");
        lcd.setCursor(14, 1);
        lcd.print(sec);
        lcd.setCursor(0, 0); lcd.print("Set Arcade");
      }
      else if (menupos2 == 1)
      {
        menupos = 2;
        lcd.setCursor(0, 0); lcd.print("Set Seting");
        delay(500);
        lcd.setCursor(0, 0); lcd.print("                ");
        lcd.setCursor(0, 0); lcd.print("< ADRR TIME Send");
      }
      delay(100);
      menupos2 = 0;
      lcd.blink();
    }


  }


  else if (menupos == 1)
  {
    if (key == BTN_RIGHT)
    {
      lcd.setCursor(0, 0); lcd.print(" Arcade >Seting");
      delay(100);
      menupos2 = 1;
    }

    if (key == BTN_LEFT)
    { lcd.setCursor(0, 0); lcd.print(">Arcade  Seting");
      delay(100);
      menupos2 = 0;
    }

    if (key == BTN_SELECT)
    {
      if (menupos2 == 0)
      {
        menupos = 1;
      }
      else if (menupos2 == 1)
      {
        menupos = 2;
        lcd.setCursor(0, 0); lcd.print("Set Seting");
        delay(500);
        lcd.setCursor(0, 0); lcd.print("                ");
        lcd.setCursor(0, 0); lcd.print("< ADRR TIME Send");
      }
      delay(100);
      menupos2 = 0;
      lcd.blink();
    }


  }

  else if (menupos == 2)
  {
    if (key == BTN_UP)
    {

      if (menupos2 == 0)
      {
        //
      }
      else if (menupos2 == 1)
      {
        RX_ADDR++;
        lcd.setCursor(2, 1);
        lcd.print(RX_ADDR );
        lcd.setCursor(2, 1);
      }
      else if (menupos2 == 2)
      {
        sec++;
        lcd.setCursor(7, 1);
        lcd.print(sec );
        lcd.setCursor(7, 1);
      }

      delay(200);
    }
    if (key == BTN_DOWN)
    {
      if (menupos2 == 0)
      {
        //
      }
      else if (menupos2 == 1)
      {
        RX_ADDR--;
        lcd.setCursor(2, 1);
        lcd.print(RX_ADDR );
        lcd.setCursor(2, 1);
      }
      else if (menupos2 == 2)
      {
        sec--;
        lcd.setCursor(7, 1);
        lcd.print(sec );
        lcd.setCursor(7, 1);
      }

      delay(200);
    } // write for zero point to massive}

    if (key == BTN_RIGHT)
    { menupos2 < 3 ? menupos2++ : menupos2 = 3;


      if (menupos2 == 0)
      {
        lcd.setCursor(0, 1);
      }
      else if (menupos2 == 1)
      {
        lcd.setCursor(2, 1);
      }
      else if (menupos2 == 2)
      {
        lcd.setCursor(7, 1);
      }
      else if (menupos2 == 3)
      {
        lcd.setCursor(13, 1); lcd.print("^"); lcd.setCursor(13, 1);
      }
      delay(200);
    } // write for zero point to massive

    if (key == BTN_LEFT)
    { menupos2 > 0 ? menupos2-- : menupos2 = 0;

      if (menupos2 == 0)
      {
        lcd.setCursor(0, 1);
      }
      else if (menupos2 == 1)
      {
        lcd.setCursor(2, 1);
      }
      else if (menupos2 == 2)
      {
        lcd.setCursor(7, 1);
      }
      else if (menupos2 == 3)
      {
        lcd.setCursor(13, 1); lcd.print("^"); lcd.setCursor(13, 1);
      }


      delay(200);
    }

    if (key == BTN_SELECT)
    {
      if (menupos2 == 0)
      {
        menupos = 0;
        menupos2 = 0;
        lcd.noCursor();
        lcd.clear();
      }
      else if (menupos2 == 3)
      {
        tx.sendData(RX_ADDR, sec);
        transmitted = 2; // поставили что идёт запрос на получение данных
      }
      delay(300);

    }
  }

}

void rainbow() {
  firstPixelHue += 256;
  for (int i = 0; i < pixels.numPixels(); i++) { // For each pixel in strip...
    int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());

    pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
  }
  pixels.show(); // Update strip with new contents
}
