#include <LiquidCrystal.h>
#include <IRremote.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define BTN_UP   1
#define BTN_DOWN 2
#define BTN_LEFT 3
#define BTN_RIGHT 4
#define BTN_SELECT 5
#define BTN_NONE 10
int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;


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
  delay(3000);
  lcd.setCursor(0, 0);
  lcd.print("                     ");
  lcd.setCursor(0, 0);
  lcd.print("Read IR");
  irrecv.enableIRIn(); // Start the receiver
}
void loop() {


  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    
    lcd.setCursor(0, 0);
    lcd.print("IR code");
    lcd.setCursor(0, 1);
    lcd.print(results.value);
    irrecv.resume(); // Receive the next value

  }
  delay(100);


}
