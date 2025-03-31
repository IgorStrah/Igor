#define ADC_PIN 0
#define FREQ_PIN 1
#define VOL_THR 20
#define VOL_MAX 250


#include <Arduino.h>



#define NUM_LEDS M_WIDTH * M_HEIGHT

#define FHT_N 32     // ширина спектра х2
#define LOG_OUT 1
#include <FHT.h>     // преобразование Хартли


uint32_t tmr, tmr2;
bool emojiState = false;
bool emoji;



int peak_cases[4][6] = {
    {8, 50, 9, 70, 10, 55}, // Кейс 1
    {9, 55, 10, 55, 11, 55},    // Кейс 2
    {2, 50, 3, 50, 3, 50},    // Кейс 3
    {5, 85, 6, 75, 10, 65}    // Кейс 4
};















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



int detect_whistle(uint8_t spectrum[], int size) {

    for (int case_index = 0; case_index < 4; case_index++) {
        int peaks = 0;
        bool valid = true;

        for (int i = 2; i < size; i++) {
            if (spectrum[i] > 40) {
                // Проверяем, соответствует ли текущая позиция одной из эталонных для текущего кейса
                bool is_peak = false;
                for (int j = 0; j < 6; j += 2) {
                    if (i == peak_cases[case_index][j] && spectrum[i] >= peak_cases[case_index][j + 1]) {
                        is_peak = true;
                        break;
                    }
                }
                if (is_peak) {
                    peaks++;
                } else {
                    valid = false; // Найден пик в неправильной позиции
                }
            }
        }

        // Если паттерн найден, проверяем на дополнительные пики
        if (peaks == 3 && valid) {
            for (int i = 2; i < size; i++) {
                if (spectrum[i] > 40) {
                    bool is_extra_peak = true;
                    for (int j = 0; j < 6; j += 2) {
                           if (i == peak_cases[case_index][j] && spectrum[i] >= peak_cases[case_index][j + 1]) {
                            is_extra_peak = false;
                            break;
                        }
                    }
                    if (is_extra_peak) {
                        return 999;
                    }
                }
            }
        }
 

        if (peaks == 3 && valid) {
            switch (case_index) {
                case 0: return 1;
                case 1: return 2;
                case 2: return 3;
                case 3: return 4;
            }
        }
    

    return 0;

/*
 for (int i = 2; i < 16; i++) {
    Serial.print("S" ); Serial.print(i);Serial.print("-" );
   //Serial.print(max(20, fht_log_out[i])-20);
   Serial.print(spectrum[i]);
  Serial.print('\t');

  }
  Serial.println();
*/




    }
}


