#define ADC_PIN 0
#define FREQ_PIN 1
#define VOL_THR 2
#define VOL_MAX 200

#define EMOJI_THR 700


#define FHT_N 32     // ширина спектра х2
#define LOG_OUT 1
#include <FHT.h>     // преобразование Хартли

#define WINDOW_SIZE 250
#define ARRAY_SIZE 10
int window[ARRAY_SIZE];
uint32_t tmr, tmr2;
bool emojiState = false;
bool emoji;


void setup() {
  Serial.begin(115200);

}

void loop() {
  byte mouthSize = filter(getSoundLength());
  Serial.print("mouthSize: ");
  Serial.println(mouthSize);
  analyzeAudio();
    for (int i = 2; i < 16; i++) {
    Serial.print(max(20, fht_log_out[i])-20);
    Serial.print('\t');
    }
    Serial.println();
  
  int freqVal = 0;
  for (int i = 2; i < 16; i++) {
    freqVal += (max(20, fht_log_out[i]) - 20) * i;
  }
  static float freqFil = 0;
  freqFil += (freqVal - freqFil) * 0.3;
  Serial.println(freqFil);

  if (millis() - tmr >= 50) {
    tmr = millis();
    static int counter = 0;
    window[counter] = freqFil;
    int thisMin = 10000, thisMax = 0;
    for (byte i = 0; i < ARRAY_SIZE; i++) {
      if (window[i] > thisMax) thisMax = window[i];
      if (window[i] < thisMin) thisMin = window[i];
    }
    counter++;
    if (counter >= ARRAY_SIZE) counter = 0;
    if (thisMin > 150 && thisMax - thisMin < WINDOW_SIZE) {
      tmr2 = millis();
      emojiState = true;
      emoji = (thisMax + thisMin) / 2 < EMOJI_THR;
    }
  }
  if (millis() - tmr2 >= 1000) {
    emojiState = false;
    tmr2 = millis();
  }

 // Serial.print(emojiState * 500);
 // Serial.print(' ');
 // Serial.println(freqFil);

  if (!emojiState) {    // если рот
   
    for (byte y = 0; y < 8; y++) {
      for (byte x = 0; x < 8; x++) {
      //  matrix.setPix(x, y, bitRead(mouth[mouthSize][y], x) ? thisColor : 0);   // рисуем

        Serial.println( mouthSize  );
delay(1000);

      }
    }
  } else {              // если эмоция
    for (byte y = 0; y < 8; y++) {
      for (byte x = 0; x < 8; x++) {
       // matrix.setPix(x, y, bitRead(emoji ? angry[y] : smile[y], x) ? emoji ? mCOLOR(RED) : mCOLOR(BLUE) : 0);
       Serial.println(" ( ");
       delay(1000);

      }
    }
  }

  delay(10);
}

int filter(int value) { // возвращает фильтрованное значение
  static int buff[3];
  static byte _counter = 0;
  int middle;
  buff[_counter] = value;
  if (++_counter > 2) _counter = 0;

  if ((buff[0] <= buff[1]) && (buff[0] <= buff[2])) {
    middle = (buff[1] <= buff[2]) ? buff[1] : buff[2];
  } else {
    if ((buff[1] <= buff[0]) && (buff[1] <= buff[2])) {
      middle = (buff[0] <= buff[2]) ? buff[0] : buff[2];
    } else {
      middle = (buff[0] <= buff[1]) ? buff[0] : buff[1];
    }
  }
  return middle;
}
