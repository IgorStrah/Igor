#define ADC_PIN 0
#define FREQ_PIN 1
#define VOL_THR 20
#define VOL_MAX 250


#include <Arduino.h>




#define FHT_N 32     // ширина спектра х2
#define LOG_OUT 1
#include <FHT.h>     // преобразование Хартли


uint32_t tmr, tmr2;
bool emojiState = false;
bool emoji;



// Структура для описания свистка
struct Whistle {
    int frequencies[3]; // Частоты свистка
    int thresholds[3];  // Пороговые значения громкости для частот
    int count;          // Количество частот в свистке (1, 2 или 3)
};

// Наборы свистков
Whistle whistles[] = {
    {{8, 9, 10}, {55, 60, 50}, 3}, // Свисток 1
    {{3, 4, -1},   {65, 55, 0},  2}, // Свисток 2
    {{7, 8, -1},  {50, 50, 0},   2}, // Свисток 3
    {{11, 12, 13},   {60, 60, 60}, 3}  // Свисток 4
};

uint8_t detect_whistle(uint8_t spectrum[], int size) {



    // Сравниваем спектр с каждым свистком
    for (int w = 0; w < sizeof(whistles) / sizeof(whistles[0]); w++) {
        Whistle whistle = whistles[w];
        int matched = 0;
        bool valid = true;

        for (int i = 0; i < whistle.count; i++) {
            int freq = whistle.frequencies[i];
            int threshold = whistle.thresholds[i];

            if (freq >= 0 && spectrum[freq] >= threshold) {
                matched++;
            } else {
                valid = false; // Если одна из частот не соответствует порогу, свисток не валиден
                break;
            }
        }

        // Если свисток распознан, проверяем на дополнительные пики
        if (matched == whistle.count && valid) {
            for (int i = 2; i < size; i++) {
                if (spectrum[i] > 50) {
                    bool is_extra_peak = true;
                    for (int j = 0; j < whistle.count; j++) {
                        if (i == whistle.frequencies[j]) {
                            is_extra_peak = false;
                           // break;
                        }
                    }
                    if (is_extra_peak) {
                        //return 888;
                    }
                }
            }

            // Возвращаем результат для текущего свистка
            switch (w) {
                case 0: return 1;
                case 1: return 2;
                case 2: return 3 ;
                case 3: return 4;
            }
        }
    }

    return 99;
}

void setup() {
    Serial.begin(115200);
   
}

void loop() {
  analyzeAudio();
    for (int i = 2; i < 16; i++) {
    Serial.print("i" ); Serial.print(i);Serial.print("-" );
   //Serial.print(max(20, fht_log_out[i])-20);
   Serial.print(fht_log_out[i]);
  Serial.print('\t');

  }
  Serial.println();
  
    
      Serial.println();
    Serial.println(detect_whistle(fht_log_out, 16));
      Serial.println();
}











/*

void loop() {

  analyzeAudio();
  
  for (int i = 2; i < 16; i++) {
    Serial.print("i" ); Serial.print(i);Serial.print("-" );
   //Serial.print(max(20, fht_log_out[i])-20);
   Serial.print(fht_log_out[i]);
  Serial.print('\t');

  }
  Serial.println();
  
    
 Serial.println(detect_whistle(fht_log_out, 16));


  
 // Serial.println(freqFil);

  if (millis() - tmr >= 50) {
    tmr = millis();
    static int counter = 0;

    counter++;
    if (counter >= 1000) counter = 0;

  }
  if (millis() - tmr2 >= 1000) {
    emojiState = false;
    tmr2 = millis();
  }

  //Serial.print(emojiState * 500);
  //Serial.print(' ');
  //Serial.println(freqFil);


 
  delay(20);
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

*/
