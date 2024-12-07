
//----- НАСТРОЙКИ -----
#define difficulty 200  // миллисекунд на реакцию (плюс минус)
#define max_knock 10    // число запоминаемых "стуков"
#define debug 1         // режим отладки - вывод в порт информации о процессе игры
//----- НАСТРОЙКИ -----


#define buzzPin 13       // пин пищалки

#include <TimerOne.h>

byte fade_count, knock=6;
volatile byte mode;
boolean cap_flag, write_start;
volatile boolean debonce_flag, threshold_flag;
volatile unsigned long debounce_time;
unsigned long last_fade, last_try, last_knock, knock_time, button_time, sound_time;

byte count, try_count;
 int  min_wait[6], max_wait[6];
#define BUTTON_PIN 2
 int wait_time[]={800,1100,400,400,400,700};

void setup() {
  Serial.begin(115200);
  if (debug) Serial.println("system start");
  delay(50);

  // настраиваем пины питания как выходы
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, 0);
 pinMode(BUTTON_PIN, INPUT);
   pinMode(3, OUTPUT);
  digitalWrite(3, 0);
   pinMode(A0, OUTPUT);
  digitalWrite(A0, 1);
  delay(50);
  attachInterrupt(digitalPinToInterrupt(2), threshold, FALLING);
  
  threshold_flag = 0;
 //knockWrite();
 for (byte i = 0; i < knock+1; i++) {
        min_wait[i-1] = wait_time[i-1] - difficulty;    // определить время с учётом времени реакции
        max_wait[i-1] = wait_time[i-1] + difficulty;  
      // delay(wait_time[i-1]);                          // ждать время шага одного хода
      // tone(buzzPin, 400, 50);                       // пыхнуть светодиодом
    
    } 
}

void knockWrite() {                                 // режим записи стука
  if (debug) Serial.println("knock write mode");
  last_knock = millis();
  knock = 0;
  while (1) {                                     // ждём первого удара
    if (millis() - last_knock > 5000) {           // если 5 секунд не ударяли
      write_start = 0;                            // всё сбросить, выйти из режима
      break;
    }
    if (threshold_flag) {                         // если ударили
      write_start = 1;                            // разрешить запись
      tone(buzzPin, 400, 50);                 // пикнуть дрыгнуть
      last_knock = millis();
      threshold_flag = 0;
      if (debug) Serial.println("knock");
      break;
    }
  }

  if (write_start) {                                  // если запись пошла
    while (1) {
      if (threshold_flag) {
        knock_time = millis() - last_knock;           // расчёт времени между стуками
        wait_time[knock] = knock_time;                // записать
        min_wait[knock] = knock_time - difficulty;    // определить время с учётом времени реакции
        max_wait[knock] = knock_time + difficulty;    // определить время с учётом времени реакции
        knock++;                                      // перейти к следующему
        tone(buzzPin, 400, 50);                                  // пикнуть дрыгнуть
        last_knock = millis();
        threshold_flag = 0;
        if (debug) Serial.println("knock");
      }
      if (millis() - last_knock > 3000) {
        break;
      }
    }
    // показать комбинацию "раунда"
    tone(buzzPin, 400, 50);                         // пыхнуть светодиодом
    for (byte i = 0; i < knock+1; i++) {
      delay(wait_time[i-1]);                          // ждать время шага одного хода
      tone(buzzPin, 400, 50);                       // пыхнуть светодиодом
      if (debug) Serial.println(wait_time[i]);
      if (debug) Serial.println(i);
    }
    mode = 0;                            // перейти в режим игры
  }

}

void loop() {


  if (millis()-sound_time > 45000)
{
digitalWrite(A0, 0);
delay(5000);
digitalWrite(A0, 1);
    sound_time=millis();
}

  if (threshold_flag && mode == 0) {
    threshold_flag = 0;
    if (knock == 0) {
      mode = 3;
      goto openCap;
    }
    debounce_time = millis();
    last_try = millis();      // обнулить таймер
    tone(buzzPin, 400, 50);
    try_count = 0;
    threshold_flag = 0;
    while (1) {

      // если не нажал в установленное время (проигрыш)
      if (millis() - last_try > max_wait[try_count]) {
        // мигнуть красным два раза
        tone(buzzPin, 222, 50);
        delay(1000);
        mode = 0;             // перейти в начало! Это начало нового раунда
        if (debug) Serial.println("too slow");
           if (debug) Serial.println(max_wait[try_count]);
        threshold_flag = 0;
         
        break;
      }
      if (threshold_flag) {

        // если нажатие попало во временной диапазон (правильное нажатие)
        if (millis() - last_try > min_wait[try_count] && millis() - last_try < max_wait[try_count]) {
          tone(buzzPin, 400, 50);               // мигнуть
          try_count++;               // увеличить счётчик правильных нажатий
          last_try = millis();       // ВОТ ТУТ СЧЁТЧИК СБРАСЫВАЕТСЯ, ЧТОБЫ УБРАТЬ ВЛИЯНИЕ ЗАДЕРЖЕК!
          threshold_flag = 0;        // сбросить флаг
          if (debug) Serial.println("good");

          // если нажал слишком рано (проигрыш)
        } else if (millis() - last_try < min_wait[try_count] && threshold_flag) {
          tone(buzzPin, 222, 50);
          delay(100);
          tone(buzzPin, 222, 50);        // мигнуть красным дважды
          delay(1000);
          mode = 0;            // перейти в начало! Это начало нового раунда
          if (debug) Serial.println("too fast");
          threshold_flag = 0;
             
          
          break;
        }

        // если число правильных нажатий совпало с нужным для раунда (выигрыш)
        if (try_count == knock) {
          // мигнуть 3 раза
          delay(200);
          tone(buzzPin, 400, 50);
          delay(200);
          tone(buzzPin, 400, 50);
          delay(200);
          tone(buzzPin, 400, 50);
          delay(200);
          mode = 3;   // перейти к действию при выигрыше
          if (debug) Serial.println("victory");
          while(1)
          {

          digitalWrite(3, 1);
          }
        }
      }
    }
  }


  if (mode == 3) {
openCap:
    mode = 4;
    delay(500);
   // open_cap();
   
  }


  if ((threshold_flag && mode == 4) || mode == 5) {
    mode = 0;
    delay(500);
  //  close_cap();
   
  }

  
}

void threshold() {
  //Serial.println("knock");
  if (millis() - debounce_time > 150) debonce_flag = 1;
  if (debonce_flag) {
    debounce_time = millis();
    threshold_flag = 1;
    debonce_flag = 0;
  }
}

void buttonPress() {
  if (mode == 4) {
    mode = 5;
  }
}


