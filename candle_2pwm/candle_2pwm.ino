/*
 * candle.c
 *
 * Created: 09.05.2016 
 * Author: Mar.lux
 *
 *
 * The candle waits for nightfall (darkness > NightlLevel), 
 * The candle then burns for some time (CandleMinutes minutes),
 * Then, the device waits for the next day and night to repeat
 *
 *
 * Calibration needed!
 * The parameters NightLevel and DayLevel change with selection of LED and voltage
 * Some software engineering might be needed to get to the right values for your specific application
 *
 *
 * Components:
 *   1x ATTiny 25/45/85 (e.g., with SOIC to DIP breakout board)
 *   1x LTL2T3YR6JS - 5mm LED, yellow, 1150mcd, 60° viewing angle or your favorite LED
 *   1x CR2032 with battery holder
 *
 *
 * Schematic 
 *
 *               _______
 *             1-|°     |-8 VCC+
 *             2-|  AT  |-7 PB2 -> LED-
 *             3-| Tiny |-6 PB1 -> LED+
 *        GND- 4-|______|-5
 *
 * 
 * Power consumption (3V):
 *
 * Waiting for day or night: 4 uA 
 * Candle burning (no LED): 190 uA
 * Candle burning (with yellow LED): 2.5 mA
 *
*/

#define CandleMinutes 1 // how long the candle burns (in minutes)
#define NightLevel 5 // darkness level for night (higher = darker)
#define DayLevel 1    // darkness level for day (lower = brighter)


#define LED  PB1 // has to be PB1, since that pin is OC0B
#define LED1 PB0
#define LED2 PB4

#define GND PB2

#define TriggerCount 1   // number of consecutive day/night measurements to detect day/night
#define MeasurementInterval 1 // delay between two light measurements (units of 4s. 30 = 2minutes)


#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

volatile uint8_t tcounter=0;

uint8_t isDarkerThan(uint8_t level);

int main(void)
{
  uint8_t counter, counter2, minutes,r;
  uint8_t flickerstrength=4,flickerstrength1=4,flickerstrength2=4,mode = 1;   // 1: day, 2: candle, 0 night;
  uint16_t RND_Num = 0x3333; // Current random number
  int8_t state=1;
  uint8_t b=170;
  // Initialize Timer 0
  //TCCR0A = (1 << WGM00) | (1 << WGM01); /* Fast PWM mode, disconnect all pins */
  TCCR0A = 2<<COM0A0 | 2<<COM0B0 | 3<<WGM00;
  TCCR0B = (0<<WGM02) | (1 << CS00); /* Fast PWM mode, disconnect all pins */
  TCCR1 = 0<<PWM1A | 0<<COM1A0 | 1<<CS10;
  GTCCR = 1<<PWM1B | 2<<COM1B0;
  TIMSK = (1 << TOIE0) ;//|(1 << TOIE1) ; // enable timer0 overflow interrupt, and only that interrupt

  WDTCR |= (1<<WDIE) | (1<<WDP2); // Enable watchdog timer interrupts and set prescale timer to 250ms

  
  sei();

  clock_prescale_set(clock_div_16);  //slow down to 500kHz
 // PRR = (1<<PRTIM1) | (1<<PRUSI) | (1<<PRADC); // save power, data sheet page 38
  ACSR = (1<<ACD); //Disable the analog comparator

  DDRB = (1<<LED) | (1<<LED1)| (1<<LED2) |(1<<GND);
  PORTB = ~(1<<GND); // Pull up resistor on unconnected pins, set LED pin high
  
  MCUCR |= (1<< SM1);//set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Use the Power Down sleep mode
  sleep_mode(); // light up LED for 0.25 seconds to indicate initialization
  PORTB &= ~(1<<LED);    // turn off LED
  PORTB &= ~(1<<LED1);    // turn off LED
  PORTB &= ~(1<<LED2);    // turn off LED

  WDTCR &= ~(1<<WDP2);  // set timer prescaler back to 16 ms

  while(1)
  {


    
    //////////////////////////////////////////////////////////////////////////
    // Wait for day and then night (on startup, wait just for night)
    //////////////////////////////////////////////////////////////////////////
    while (mode < 2)
    {
      counter = 0;
      while (counter < TriggerCount)
      {
        WDTCR |= (1<<WDP3); // set prescale timer to 4 s
        r = MeasurementInterval+1;
        while (--r) sleep_mode(); // wait until next light measurement
  PORTB |=(1<<LED1);    // turn off LED
  PORTB |=(1<<LED2);    // turn off LED
        
        WDTCR &= ~(1<<WDP3);  // set prescale timer back to 16 ms
        if (mode)
        {
          if (isDarkerThan(NightLevel)) counter++;
          else if (counter) counter--;
        }
        else
        {
          if (!isDarkerThan(DayLevel)) counter++;
          else if (counter) counter--;
        }
      }
      mode++;
    }
    
    WDTCR &= ~(1<<WDIE);  // Disable watchdog timer interrupts
    
    //////////////////////////////////////////////////////////////////////////
    // Let the candle burn
    //////////////////////////////////////////////////////////////////////////
    counter2 = 0;

    // Enable candle
    MCUCR &= ~(1<< SM1); //set_sleep_mode(SLEEP_MODE_IDLE); // + IDLE 186 uA
    TCCR0A |= (1 << COM0B1); /* Clear OC0B on compare match, set at BOTTOM */
    TCCR0A |= (1 << COM0A1); /* Clear OC0B on compare match, set at BOTTOM */
    GTCCR  |=  (1 << COM1B1); // disconnect pin
  //  GTCCR  |=  (1 << COM1B0); // disconnect pin
    //GTCCR = 1<<PWM1B | 2<<COM1B0;  
    TCCR0B = (1 << CS01);   /* Clock with /8 prescaler */

  
 



    minutes=CandleMinutes;
    while (minutes)
    {
      while (tcounter) sleep_mode();
      RND_Num = 2053 * RND_Num + 13849; // get new random number
      if (++counter2 == 0) // once per minute
      {
        minutes--;
        flickerstrength= 4 + (RND_Num >> 14); // 1 ... 4
        flickerstrength1= 2 + (RND_Num >> 4); // 1 ... 4
        flickerstrength2= 1 + (RND_Num >> 4); // 1 ... 4
      }

      r = (RND_Num >> 13); // 0 ... 7
      state = -state;
      OCR0B = 80+2*flickerstrength*(7-r)*state; // 80 +/- 56 -> 24 .. 136
      OCR0A = 80+2*flickerstrength1*(7-r)*state; // 80 +/- 56 -> 24 .. 136
      OCR1B = 80+2*flickerstrength2*(7-r)*state; // 80 +/- 56 -> 24 .. 136

      tcounter = ((r*r+2*r+4) << 1);
    }

    TCCR0A &= ~(1 << COM0B1); // disconnect pin
    TCCR0A &= ~(1 << COM0A1); // disconnect pin
    GTCCR &= ~(1 << COM1B1); // disconnect pin
  //  GTCCR &= ~(1 << COM1B0); // disconnect pin
    TCCR0B = 0; //&= ~(1 << CS01);   // disconnect clock

    MCUCR |= (1<< SM1); //set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Use the Power Down sleep mode
    WDTCR |= (1<<WDIE); // Enable watchdog timer interrupts
    mode = 0; // night mode
  }
}



uint8_t isDarkerThan(uint8_t level)
{
  //Assumptions: WDT enabled and configured to sleep for 16ms
  
  // Reverse charge LED
  PORTB = ~((1<<LED)|(1<<LED1)|(1<<LED2)); // pull LED pin low, all other pins high

  sleep_mode();
  
  //Setup measurement
  DDRB &= ~(1<<GND); // set GND as input
  PORTB &= ~(1<<GND); // turn off GND pull-up

  while (level--) sleep_mode();
  DDRB |= (1<<GND); // set GND as output
  return bit_is_set(PINB, GND);
}

EMPTY_INTERRUPT(WDT_vect)

ISR(TIM0_OVF_vect) {
  tcounter--; // increments at 244 Hz
}


/*

 cli();
    DDRB = (1<<LED) | (1<<LED1)| (1<<LED2) |(1<<GND);
   TCCR0A = 2<<COM0A0 | 2<<COM0B0 | 3<<WGM00;
   TCCR0B = 0<<WGM02 | 1<<CS00;
    TCCR1 = 0<<PWM1A | 0<<COM1A0 | 1<<CS10;
   GTCCR = 1<<PWM1B | 2<<COM1B0;
//sei();
 for (;;)
    {
        

        if (++b>255) b=0;
          OCR1B = b;
   
        _delay_ms(1);
    }
    */
