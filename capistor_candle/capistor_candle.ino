/*

   Created: 09.05.2016
   Author: Mar.lux
*/

#define CandleMinutes 700 // how long the candle burns (in minutes)



#define LED  PB1 // has to be PB1, since that pin is OC0B
#define LED0 PB2
#define LED1 PB0
#define LED2 PB4

#define TriggerCount 1   // number of consecutive day/night measurements to detect day/night
#define MeasurementInterval 1 // delay between two light measurements (units of 4s. 30 = 2minutes)
#define ANALOG_IN PB3 //ADC2



#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

volatile uint8_t tcounter = 0;

uint8_t isDarkerThan(uint8_t level);

int main(void)
{
  uint8_t counter, counter2, r;
  int flickerstrength = 4, flickerstrength1 = 4, flickerstrength2 = 4, mode = 1; // 1: day, 2: candle, 0 night;
  uint8_t flickermp = 1, flickermp1 = 1, flickermp2 = 1; // PWM speed multiplier
  uint8_t flickercount = 0, flickercount1 = 0, flickercount2 = 0; // 0: up, 1: down;
  uint16_t RND_Num = 0x3333,minutes; // Current random number
  uint8_t state = 0,offon; 
  uint8_t b = 170;
  
  // Initialize Timer 0
  TCCR0A = (1 << WGM00) | (1 << WGM01)| (1 << WGM02); /* Fast PWM mode, disconnect all pins */
  TCCR0A = 2 << COM0A0 | 2 << COM0B0 | 3 << WGM00;
  TCCR0B = (0 << WGM02) | (1 << CS00); /* Fast PWM mode, disconnect all pins */
  TCCR1 = 0 << PWM1A | 0 << COM1A0 | 1 << CS10;
  GTCCR = 1 << PWM1B | 2 << COM1B0;
  TIMSK = (1 << TOIE0) ;//|(1 << TOIE1) ; // enable timer0 overflow interrupt, and only that interrupt
  WDTCR |= (1 << WDIE) | (1 << WDP2); // Enable watchdog timer interrupts and set prescale timer to 250ms
  DDRB &= ~(1 << ANALOG_IN); // вход = 0

  ADMUX = 0 << REFS0 | 0 << REFS1    |   1 <<  ADLAR | 1 << MUX0| 1 << MUX1| 0 << MUX2| 0 << MUX3; 
  ADCSRA= 1 << ADEN | 1 << ADPS0 | 1 << ADPS1| 1 << ADPS2;
 // ADCSRB = 0x00;
  DIDR0 |= (1 << ANALOG_IN); 
  clock_prescale_set(clock_div_16); 
  sei();

  // clock_prescale_set(clock_div_16);  //slow down to 500kHz
  //PRR = (1<<PRTIM1) | (1<<PRUSI) | (1<<PRADC); // save power, data sheet page 38
  ACSR = (1 << ACD); //Disable the analog comparator
  DDRB = (1 << LED) | (1 << LED0) | (1 << LED1) | (1 << LED2);
  
  MCUCR |= (1 << SM1); //set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Use the Power Down sleep mode
  sleep_mode(); // light up LED for 0.25 seconds to indicate initialization
  PORTB &= ~(1 << LED);  // turn off LED
  PORTB &= ~(1 << LED0);  // turn off LED
  PORTB &= ~(1 << LED1);  // turn off LED
  PORTB &= ~(1 << LED2);  // turn off LED

  WDTCR &= ~(1 << WDP2); // set timer prescaler back to 16 ms
state=0;
flickerstrength =1; 
flickerstrength1 =2;
flickerstrength2 =3;

  while (1)
  {
  if (get_adc() > 6)  
  { state++; sleep_mode();   }
  if (state>=35) {(offon==0)?offon=1:offon=0; state=0;  }
    
    
  if (offon==1)
  {
    WDTCR &= ~(1 << WDP2); // set timer prescaler back to 16 ms
    MCUCR &= ~(1 << SM1); //set_sleep_mode(SLEEP_MODE_IDLE); // + IDLE 186 uA
    TCCR0A = 2 << COM0A0 | 2 << COM0B0 | 3 << WGM00;
    TCCR0B = (1 << CS01);   /* Clock with /8 prescaler */
    GTCCR = 1 << PWM1B | 2 << COM1B0;
    minutes = CandleMinutes;
    
while (minutes)
{
  if (get_adc() > 6)  
  { state++; sleep_mode();   }
  if (state>=25) {minutes=0;offon=0; state=0; WDTCR |= (1 << WDP2); sleep_mode(); }






RND_Num = (2053 * RND_Num + 13849)/4; // get new random number
      r = (1 + (RND_Num >> 14));
if ((flickercount==0))     // count up 
{
  flickerstrength>=150?flickercount=1:flickercount=0;
  flickerstrength+=r;
}
else if ((flickercount==1))
{
  flickerstrength<=-100?flickercount=0:flickercount=1;
  flickerstrength-=r;
}


RND_Num = (2053 * RND_Num + 13849)/4; // get new random number
 r = (1 + (RND_Num >> 14));
if ((flickercount1==0))     // count up 
{
  flickerstrength1>=100?flickercount1=1:flickercount1=0;
  flickerstrength1+=r;
}
else if ((flickercount1==1))
{
  flickerstrength1<=-100?flickercount1=0:flickercount1=1;
  flickerstrength1-=r;
}
RND_Num = (2053 * RND_Num + 13849)/4; // get new random number
      r = (1 + (RND_Num >> 13));
if ((flickercount2==0))     // count up 
{
  flickerstrength2>=150?flickercount2=1:flickercount2=0;
  flickerstrength2+=r;
}
else if ((flickercount2==1))
{
  flickerstrength2<=-100?flickercount2=0:flickercount2=1;
  flickerstrength2-=r;
}








  
     // OCR0B = flickerstrength<1?5:flickerstrength;
     // OCR0A = flickerstrength1<1?5:flickerstrength1;
     // OCR1B = flickerstrength2<1?5:flickerstrength2;
      OCR1B =1;



      
     while (tcounter) sleep_mode();
     RND_Num = (2053 * RND_Num + 13849)/4; // get new random number
     r = (RND_Num >> 13); // 0 ... 7
     tcounter = ((r*r+2*r+4) << 1);
     if (++counter2 == 0)  minutes--;



     
}
  }


    flickercount=0;flickercount1=0;flickercount2=0;
    TCCR0A &= ~(1 << COM0B1); // disconnect pin
    TCCR0A &= ~(1 << COM0A1); // disconnect pin
    GTCCR &= ~(1 << COM1B1); // disconnect pin
    GTCCR &= ~(1 << COM1B0); // disconnect pin
    TCCR0B  =0;//&= ~(1 << CS01);   // disconnect clock
  PORTB &= ~(1 << LED);  // turn off LED
  PORTB &= ~(1 << LED1);  // turn off LED
  PORTB &= ~(1 << LED2);  // turn off LED
    MCUCR |= (1 << SM1); //set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Use the Power Down sleep mode
    WDTCR |= (1 << WDIE); // Enable watchdog timer interrupts
    offon = 0; // night mode
  }
}




EMPTY_INTERRUPT(WDT_vect)

ISR(TIM0_OVF_vect) {
  tcounter--; // increments at 244 Hz

}


uint8_t get_adc()
{
 ADCSRA |= (1 << ADSC);           // start ADC measurement
 return ADCH ; // return the inverted 8-bit left adjusted adc val

}
