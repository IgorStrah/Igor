#include "GyverBus.h"


#define TX_PIN 2    // пин
#define TX_ADDR 1   // наш адрес
#define RX_ADDR1 2   // адрес приёмника
// пин 4, адрес 1, буфер на 15 байт
GBus<TX_PIN, GBUS_FULL> tx(TX_ADDR, 15);

#include <LiquidCrystal.h>
#include <IRremote.h>

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
String inputi2c = "";

byte Flstart = 0;

int sec = 0;
byte numdev = 0x00;
byte timer = 0;
byte menupos = 0;
int button;
int val;
int transmitted = 0; // 0 wait/ 1 request/ 2 send
int RX_ADDR;    // адрес приёмника
int error = 0;
int detectButton() {
  int keyAnalog =  analogRead(A0);
  if (keyAnalog < 100) {
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

  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Arduino Master");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("                     ");
  lcd.setCursor(0, 0);
  lcd.print("Read IR");
  irrecv.enableIRIn(); // Start the receiver

  randomSeed(analogRead(0));

}
void loop() {
  int button = detectButton();
  int nDevices = 0;


  if ((tx.tick() == TX_COMPLETE) && (transmitted == 2))
  {
    transmitted = 1;
  }





  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    lcd.setCursor(0, 0);
    lcd.print("IR code");
    lcd.setCursor(0, 0);
    lcd.print("              ");
    lcd.setCursor(0, 0);
    lcd.print(results.value);
    irrecv.resume(); // Receive the next value

  }

  
  // если нам что-то отправили
  if (error == 3)
  {
    error = 0;
    transmitted = 0;
  }


  static uint32_t tmr;
  if (millis() - tmr > 100) {
    tmr = millis();

    if (transmitted == 1)
    {
      byte state = tx.sendRequestAck(RX_ADDR, 2, 500);
      switch (state) {
        case ACK_ERROR: Serial.println("ack error");
          error++;
          break;
        case ACK_ONLY: Serial.println("got ack");
          break;
        case ACK_DATA: Serial.print("got data: ");
          int val;
          tx.readData(val);
          Serial.println(val);
          lcd.setCursor(8, 1);
          lcd.print("Dat");
          lcd.setCursor(12, 1);
          lcd.print("  ");
          lcd.setCursor(13, 1);
          lcd.print(val);
          if ((val == 0) ||(val == 1) || (val == 99)|| (val == 98))
          {
            transmitted = 0;
          }
          break;

      }
    }

  }

}



void menu( int key)
{
  
  if (key == BTN_UP)
  { sec++;

    lcd.setCursor(0, 0);
    lcd.print("set sec        ");
    lcd.setCursor(8, 0);
    lcd.print(sec);
    delay(100);
  } 
  if (key == BTN_DOWN)
  { sec--;
    lcd.setCursor(0, 0);
    lcd.print("set sec        ");
    lcd.setCursor(8, 0);
    lcd.print(sec );
    delay(100);
  } // write for zero point to massive}
  
  if (key == BTN_RIGHT)
  { RX_ADDR++;
    lcd.setCursor(0, 0); lcd.print("set adress        ");
    lcd.setCursor(12, 0); lcd.print(RX_ADDR );
    transmitted=0;
    delay(100);
  } // write for zero point to massive

  if (key == BTN_LEFT)
  { RX_ADDR--;
    lcd.setCursor(0, 0); lcd.print("set adress        " );
    lcd.setCursor(12, 0); lcd.print(RX_ADDR );
    transmitted=0;
    delay(100);
  } // write for zero point to massive

  if (key == BTN_SELECT)
  { lcd.setCursor(0, 0); lcd.print("                " );
    lcd.setCursor(0, 0); lcd.print("RX_ADDR " );
    lcd.setCursor(8, 0); lcd.print(RX_ADDR );
    lcd.setCursor(10, 0); lcd.print("sec" );
    lcd.setCursor(14, 0); lcd.print(sec );

    tx.sendData(RX_ADDR, sec);
    transmitted = 2; // поставили что идёт запрос на получение данных
    delay(100);

  }

}
