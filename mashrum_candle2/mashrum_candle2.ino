// avr-libc library includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
uint8_t analog_ref = DEFAULT;
const byte pwm_pins[9] = {PD2, PD3, PD4, PD5, PD6, PD7, PB0, PB1, PB2};

#define PWM_count sizeof(pwm_pins)


volatile byte flickercount[PWM_count];
volatile int pwm_value[PWM_count];

#define CandleMinutes 2// how long the candle burns (in minutes)
unsigned long period_time = 6000;
unsigned long period_time1 = 500;
volatile unsigned long schedulecounter;
uint8_t clk, mode; //0-wiat/1-day/2-night
int8_t counter, counter2, minutes, r;
byte pwm_counter;
byte flickerstrength = 0;



volatile int currentSensorValue1 = 0;
volatile int currentSensorValue2 = 0;
volatile int analogPin = A0;
volatile bool trueValue = false;

void setup()
{



  TCCR1A = 0; // установить регистры в 0
  TCCR1B = 0;

  OCR1A = 10; // установка регистра совпадения

  TCCR1B |= (1 << WGM12); // включение в CTC режим

  // Установка битов CS10 и CS12 на коэффициент деления 64
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS11);

  TIMSK1 |= (1 << OCIE1A);  // включение прерываний по совпадению
  
  DDRD = 1 << PD2 | 1 << PD3 | 1 << PD4 | 1 << PD5 | 1 << PD6 | 1 << PD7;
  PORTD  = 0 << PD2 | 0 << PD3 | 0 << PD4;
  DDRB = 1 << PB0 | 1 << PB1 | 1 << PB2;
  PORTB = 0 << PB0 | 0 << PB1 | 0 << PB2;

  
  ADCSRA = 0;             // Сбрасываем регистр ADCSRA
  ADCSRB  |= (1 << ADTS0);             // Сбрасываем регистр ADCSRB
  ADMUX |= (1 << REFS0);  // Задаем ИОН

  ADMUX |= (1 << ADLAR);  // Меняем порядок записи бит, чтобы можно было читать только 8 бит регистра ADCH
  // Таким образом отсекаются 2 последних "шумных" бита, результат 8-битный (0...255)

  analog_ref = ADMUX;     // Запоминаем состояние регистра - оно будет использоваться при смене пина входящего сигнала

  ADMUX |= (0 & 0x07);    // Выбираем пин A0 для преобразования
  // Устанавливаем предделитель - 16 (ADPS[2:0]=100)
  ADCSRA |= (1 << ADPS2);                     //Биту ADPS2 присваиваем единицу
  ADCSRA &= ~ ((1 << ADPS1) | (1 << ADPS0));  //Битам ADPS1 и ADPS0 присваиваем нули

  ADCSRA |= (1 << ADATE); // Включаем автоматическое преобразование
  ADCSRA |= (1 << ADIE);  // Разрешаем прерывания по завершении преобразования
  ADCSRA |= (1 << ADEN);  // Включаем АЦП
  ADCSRA |= (1 << ADSC);  // Запускаем преобразование

sei(); // включить глобальные прерывания



 
  
  minutes = 20;
  sei();  // включить глобальные прерывания
  // set srart flickerstrength
  for (byte i = 0; i < PWM_count; i++)
  {
    pwm_value[i]  = random(-15     , 5);

  }
  Serial.begin(57600);
  Serial.println("Goodnight moon!");
}

void loop()
{

   
  if (schedulecounter>2) {
   
      minutes = CandleMinutes;
      flickerstrength++;
    
  
     Serial.print("currentSensorValue1 ");
     Serial.print(currentSensorValue1);
     Serial.print("     currentSensorValue2  ");
     Serial.println(currentSensorValue2);
     schedulecounter = 0;
     sleep_mode();
    
  if (currentSensorValue1 > 150)
  {
   
  
      for (byte i = 0; i < PWM_count; i++)
      {
        r =1;// random(-1, 3);
        if ((flickercount[i] == 0))
        {
          pwm_value[i] >= 210 ? flickercount[i] = 1 : flickercount[i] = 0;
          pwm_value[i] += r;
     Serial.print("pwm_value ");
     Serial.println(pwm_value[1] );
        }
        else if ((flickercount[i] == 1))
        {
          pwm_value[i] <= -10 ? flickercount[i] = 0 : flickercount[i] = 1;
          pwm_value[i] -= r;
    // Serial.print("pwm_value ");
     //Serial.println(pwm_value[1] );  
        }
        
     
      }
      //}

    }
  
 
 else 
 {
    for (byte i = 0; i < PWM_count; i++)
  {
    pwm_value[i]  = 0;

  }
   

  } 
 }
}
 
//}


ISR(ADC_vect) {
cli();
  if (trueValue) {
    int result = (ADCL >> 6) | (ADCH << 2); // Получаем 10-битный результат
    if (analogPin == A0) {            // Если актуальный входной пин A0, то присваиваем значение соответствующей переменной

      // it a photoresistor
      currentSensorValue1 = result;
    }
    else {
      // it a touch button
      currentSensorValue2 = result;
    
    }

    analogPin += 1;                   // Перебираем входные пины по кругу (А0...А1 - их может быть больше)
    if (analogPin > A1) {             // Все нужные перебрали...
      analogPin = A0;                 // ...возвращаемся к первому
    };

    int pin = analogPin;              // Приводим пин вида A0 (14) к удобному для регистра ADMUX - 0. A0=14, A1=15, ...
    if (pin >= 14) pin -= 14;

    ADMUX = analog_ref | (pin & 0x07);// Устанавливаем новый вход для преобразования
    trueValue = false;                // Устанавливаем флаг смены входного пина - следующее прерывание пропускаем
  }
  else {
    trueValue = true; // Первый раз пропускаем считывание и устанавливаем флаг на чтение в следующий раз
  }
sei();
}


ISR(TIMER1_COMPA_vect)
{
  schedulecounter++;
  pwm_counter++;
  for (byte i = 0; i < PWM_count; i++)
  {
    if (i <= 5) {
      pwm_counter > pwm_value[i] ? PORTD &= ~(1 << pwm_pins[i]) : PORTD |= (1 << pwm_pins[i]);
    }
    else      {
      pwm_counter >  pwm_value[i] ? PORTB &= ~(1 << pwm_pins[i]) : PORTB |= (1 << pwm_pins[i]);
    }
  }

}
ISR (WDT_vect) {
  wdt_disable();
 schedulecounter++;
  ADCSRA |= (1 << ADSC);
}
