#define PIN 13        // пин DI
#define NUM_LEDS 46  // число диодов
#include "Adafruit_NeoPixel.h"
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
#include <CapacitiveSensor.h>
int numpin;
int mode;
int step_word = 0;

int myprases[8] = {0, 7, 11, 16, 22, 28, 34, 39};


const char *words[]  = {
  "Hi ",   // 0
  "hiding ",     // 1
  "beware ",     // 2
  "dont even think ",     // 3
  "you can do it ",     // 4
  "dont break promises ",     // 5
  "lie to survive ",     // 6
  "they are coming ",     // 7
  "its not them ",     // 8
  "no help ",     // 9
  "last chance ",     // 10
  "youre late " ,     // 11
  "Relax",     // 12
  "they know everything ",     // 13
  "look for signs ",     // 14
  "answer is within you ",     // 15
  "you can do it",     // 16
  "beware",     // 17
  "you can do it",     // 18
  "fix your life ",     // 19
  "your hand is with me ",     // 20
  "the demon inside says yes ",     // 21
  "im here ",     // 22
  "im listening ",     // 23
  "i cannot answer ",     // 24
  "tonight ",     // 25
  "no time ",     // 26
  "all lies ",     // 27
  "scream louder ",     // 28
  "the truth is near ",     // 29
  "truths do not exist ",     // 30
  "just ask ",     // 31
  "just breathe ",     // 32
  "tomorrow at 1 ",     // 33
  "tomorrow at 2 ",     // 34
  "tomorrow at 3 ",     // 35
  "tomorrow at 4 ",     // 36
  "tomorrow at 5 ",     // 37
  "tomorrow at 6 ",     // 38
  "tomorrow at 7 ",     // 39
  "tomorrow at 8 ",     // 40
  "tomorrow at 9 ",     // 41
  "tomorrow at 10 ",     // 42
  "tomorrow at 11 ",     // 43
  "tomorrow at 12 ",     // 44
  "tomorrow ",     // 45
  "today at 1 ",     // 46
  "today at 2 ",     // 47
  "today at 3 ",     // 48
  "today at 4 ",     // 49
  "today at 5 ",     // 50
  "today at 6 ",     // 51
  "today at 7 ",     // 52
  "today at 8 ",     // 53
  "today at 9 ",     // 54
  "today at 10 ",     // 55
  "today at 11 ",     // 56
  "today at 12 ",     // 57
  "today at 5 ",     // 58
  "today at 6 ",     // 59
  "today ",     // 60
  "dig deeper ",     // 61
  "face your fear ",     // 62
  "have faith ",     // 63
  "you are loved ",     // 64
  "you are worthy ",     // 65
  "dont look back ",     // 66
  "its not worth it ",     // 67
  "its worth it ",     // 68
  "be patient ",     // 69
  "behind you ",     // 70
  "hide under the bed ",     // 71
  "wash your hands ",     // 72
  "nothing matters ",     // 73
  "bring your towel ",     // 74
  "42 ",     // 75

};


const char *names[]  = {
  "Period",   // 0
  "Work",     // 1
  "Stop",     // 2
};
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4, 2);
void setup() {
  strip.begin();
  strip.setBrightness(150);    // яркость, от 0 до 255
  strip.clear();                          // очистить
  strip.show();                           // отправить на ленту
  randomSeed(analogRead(0));

}
void loop() {

  long total1 =  cs_4_2.capacitiveSensor(40);


  if (total1 > 200)
  { // onn
    delay(100);
    mode++;
    // effect_move_right();
    effect_move_round();
  }
  else
  {
    if (mode > 20)
    {
      mode = 0;
      int cloc_num = random(4, 14);
      for (int i = 0; i < cloc_num; i++ )
      {
        effect_move_round();
        delay(110 * i);
      }
      delay(3000);







      for (int i = 254; i > 1;  i-- ) {
        strip.setPixelColor(myprases[step_word], i, i, 0);
        strip.show();                           // отправить на ленту
        delay(3);
      }








    }
    else if (mode > 3)
    {
      phrases();
      mode = 0;
    }


  }



}



void phrases()
{


  effect_move_right();
  // получаем вторую строку из массива фраз

  String worlds_s = words[random(74)];
  //   delay(1133);
  for (int i = 0; i < worlds_s.length();  i++ ) {  // перебираем буквы в фразе.

    // смотрим - если пробем - мигаем всем что у нас уже мигало
    if ((worlds_s.charAt(i)) == ' ')
    {
      for (int i2 = 0; i2 < 144; i2++ )
      { // от 0 до первой трети

        for (int i1 = 0; i1 < i;  i1++ )
        {
          send_num_world(worlds_s.charAt(i1));
          strip.setPixelColor(numpin, i2, i2, 0);   // залить красным
          strip.show();                         // отправить на ленту
          delay(1);
        }
      }
      for (int i2 = 144; i2 > 1; i2-- )
      { // от 0 до первой трети

        for (int i1 = 0; i1 < i;  i1++ )
        {
          send_num_world(worlds_s.charAt(i1));
          strip.setPixelColor(numpin, i2, i2, 0);   // залить красным
          strip.show();                         // отправить на ленту

          delay(1);
        }
      }


    }

    else
    {
      send_num_world(worlds_s.charAt(i));     // получаем номер пикселя

      // получили номер пина котоорым мигать
      // после этого включаем

      for (int i2 = 0; i2 < 144; i2++ ) {   // от 0 до первой трети
        strip.setPixelColor(numpin, i2, 0, i2);   // залить красным
        strip.show();                         // отправить на ленту
        delay(3);
      }
      for (int i3 = 144; i3 > 1; i3-- ) {   // от 0 до первой трети
        strip.setPixelColor(numpin, i3, 0, i3);   // залить красным
        strip.show();                         // отправить на ленту
        delay(3);
      }

    }


    delay(100);

  }





  // заливаем чёрным
  for (int i = 0; i < NUM_LEDS; i++ ) {   // всю ленту
    strip.setPixelColor(i, 0x000000);     // залить чёрным
    strip.show();                         // отправить на ленту
    delay(10);
  }


  for (int i = NUM_LEDS ; i < NUM_LEDS; i++ ) {   // от 1/3 до 2/3
    strip.setPixelColor(i, 0x00ff00);     // залить зелёным
    strip.show();                         // отправить на ленту
    delay(33);
  }
  for (int i = NUM_LEDS ; i < NUM_LEDS; i++ ) {   // от 2/3 до конца
    strip.setPixelColor(i, 0x0000ff);     // залить синим
    strip.show();                         // отправить на ленту
    delay(33);
  }

  // заливаем белым
  for (int i = 0; i < NUM_LEDS; i++ ) {   // всю ленту
    strip.setPixelColor(i, 0xffffff);     // залить белым
    strip.show();                         // отправить на ленту
    delay(33);
  }
  delay(33);
  // заливаем чёрным
  for (int i = 0; i < NUM_LEDS; i++ ) {   // всю ленту
    strip.setPixelColor(i, 0x000000);     // залить чёрным
    strip.show();                         // отправить на ленту
    delay(33);
  }

}



void effect_move_right()
{

  for (int i = 0; i < 11; i++ ) {
    strip.setPixelColor(33 + i, 255, 255, 0);   // залить чёрным

    strip.setPixelColor(33 - i, 255, 255, 0);   // залить чёрным

    strip.show();                         // отправить на ленту
    delay(33);
  }
    for (int i2 = 0; i2 < 12; i2++ ) {
      strip.setPixelColor(i2, 255, 255, 0);     // залить чёрным
      strip.setPixelColor(22 - i2, 255, 255, 0);   // залить чёрным

      strip.show();                         // отправить на ленту
      delay(33);
    }
  
  for (int i = 0; i < 11; i++ ) {
    strip.setPixelColor(33 + i, 0, 0, 0);   // залить чёрным

    strip.setPixelColor(33 - i, 0, 0, 0);   // залить чёрным

    strip.show();                         // отправить на ленту
    delay(33);
  }
  for (int i2 = 0; i2 < 12; i2++ ) {
    strip.setPixelColor(i2, 0, 0, 0);     // залить чёрным
    strip.setPixelColor(22 - i2, 0, 0, 0);   // залить чёрным

    strip.show();                         // отправить на ленту
    delay(33);
  }



}


void effect_move_round()
{

  strip.setPixelColor(myprases[step_word], 0, 0, 0);     // залить чёрным
  step_word >= 7 ? step_word = 0 : step_word++;
  strip.show();                           // отправить на ленту
  delay(33);
  strip.setPixelColor(myprases[step_word], 255, 255, 0);
  strip.show();                           // отправить на ленту
  delay(33);


}

char send_num_world(char char_word) {

  switch (char_word) {
    case 'a':
      numpin = 29;
      break;
    case 'b':
      numpin = 30;
      break;
    case 'c':
      numpin = 31;
      break;
    case 'd':
      numpin = 32;
      break;
    case 'e':
      numpin = 33;
      break;

    case 'f':
      numpin = 35;
      break;
    case 'g':
      numpin = 36;
      break;
    case 'h':
      numpin = 37;
      break;
    case 'i':
      numpin = 38;
      break;
    case 'j':
      numpin = 40;
      break;
    case 'k':
      numpin = 41;
      break;
    case 'l':
      numpin = 42;
      break;
    case 'm':
      numpin = 43;
      break;
    case 'n':
      numpin = 1;
      break;
    case 'o':
      numpin = 2;
      break;
    case 'p':
      numpin = 3;
      break;
    case 'q':
      numpin = 4;
      break;
    case 'r':
      numpin = 5;
      break;
    case 's':
      numpin = 6;
      break;
    case 't':
      numpin = 8;
      break;
    case 'u':
      numpin = 9;
      break;
    case 'v':
      numpin = 10;
      break;
    case 'w':
      numpin = 12;
      break;
    case 'x':
      numpin = 13;
      break;
    case 'y':
      numpin = 14;
      break;
    case 'z':
      numpin = 15;
      break;
    case '0':
      numpin = 17;
      break;
    case '9':
      numpin = 18;
      break;
    case '8':
      numpin = 19;
      break;
    case '7':
      numpin = 20;
      break;
    case '6':
      numpin = 21;
      break;
    case '5':
      numpin = 23;
      break;
    case '4':
      numpin = 24;
      break;
    case '3':
      numpin = 25;
      break;
    case '2':
      numpin = 26;
      break;
    case '1':
      numpin = 27;
      break;
    case ' ':
      numpin = 45;
      break;


      return numpin;
  }
}
