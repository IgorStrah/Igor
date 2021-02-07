
#define CandleMinutes 200 // how long the candle burns (in minutes)

const byte pwm_pins[3] = {PB0, PB1, PB4};
#define PWM_count sizeof(pwm_pins)
volatile byte flickercount[PWM_count];
volatile int pwm_value[PWM_count];
#define LED  PB1 // has to be PB1, since that pin is OC0B
#define LED0 PB4
#define LED1 PB0
#define KEY  PB2
#define TriggerCount 1   // number of consecutive day/night measurements to detect day/night
#define MeasurementInterval 1 // delay between two light measurements (units of 4s. 30 = 2minutes)
#define ANALOG_IN PB3 //ADC2

#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

volatile uint8_t tcounter = 0, Day;

uint8_t isDarkerThan(uint8_t level);

int main(void)
{
  uint8_t counter, counter2;
  int  r;

  uint16_t RND_Num = 0x3333,minutes; // Current random number
  uint8_t state = 0,offon;
  uint8_t b = 170;
  
  // Initialize Timer 0
  TCCR0A = (1 << WGM00) | (1 << WGM01)| (1 << WGM02); /* Fast PWM mode, disconnect all pins */
 // !!!!!!!TCCR0A = 2 << COM0A0 | 2 << COM0B0 | 3 << WGM00;
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
  DDRB |= (1 << LED) | (1 << LED0) | (1 << LED1);

  MCUCR |= (1 << SM1); //set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Use the Power Down sleep mode
  sleep_mode(); // light up LED for 0.25 seconds to indicate initialization
  offon=0;
  Day=1;
  WDTCR &= ~(1 << WDP2); // set timer prescaler back to 16 ms
  state=0;
 for (byte i = 0; i < PWM_count; i++)
  {pwm_value[i]  = random(-300     , 3);}
  while (1)
  {

/*
while (1)
{
 // if (get_adc() < 150) PORTB |= (1 << LED);
 // else PORTB &= ~(1 << LED);
  if (get_adc() > 190) PORTB |= (1 << LED1);//- light
  else PORTB &= ~(1 << LED1);
}
*/


WDTCR |= (1 << WDP1);  
WDTCR |= (1 << WDP2); 
sleep_mode();
WDTCR &=~ (1 << WDP2);
WDTCR &=~ (1 << WDP1);

    while(offon==0)
 {


  
  if (get_adc()> 190) {(Day>10)?Day=10:Day++;}
  
  if (Day>9)
{
  if (get_adc() < 150)  {state++; }
}
  if (bit_is_set(PINB,2) ==0) { state++; }
  if (state>=5) {offon=1; state=0;Day=0; }
     sleep_mode();
     
 }

   
    WDTCR &= ~(1 << WDP2); // set timer prescaler back to 16 ms
    MCUCR &= ~(1 << SM1); //set_sleep_mode(SLEEP_MODE_IDLE); // + IDLE 186 uA
    TCCR0A = 2 << COM0A0 | 2 << COM0B0 | 3 << WGM00;
    TCCR0B = (1 << CS01);   /* Clock with /8 prescaler */
    GTCCR = 1 << PWM1B | 2 << COM1B0;
    minutes = CandleMinutes;
     for (byte i = 0; i < PWM_count; i++)
  {pwm_value[i]  = random(-200     , 3);}


// delay
//WDTCR |= (1 << WDP1);  
//WDTCR |= (1 << WDP2); 
sleep_mode();
//WDTCR &=~ (1 << WDP2);
//WDTCR &=~ (1 << WDP1);

  
while (minutes)
{
if (bit_is_set(PINB,2) ==0) state++; 
  if (state>=50) {minutes=0;offon=0; state=0; WDTCR |= (1 << WDP2); sleep_mode(); WDTCR &=~ (1 << WDP2);}


      for (byte i = 0; i < PWM_count; i++)
      {
        r =  random(-1, 3);
        if ((flickercount[i] == 0))
        {
          pwm_value[i] >= 210 ? flickercount[i] = 1 : flickercount[i] = 0;
          pwm_value[i] += r;
        }
        else if ((flickercount[i] == 1))
        {
          pwm_value[i] <= -10 ? flickercount[i] = 0 : flickercount[i] = 1;
          pwm_value[i] -= r;
        }
      }
      OCR0B = (pwm_value[0]<1)?3:pwm_value[0];
      OCR0A = (pwm_value[1]<1)?3:pwm_value[1];
      OCR1B = (pwm_value[2]<1)?3:pwm_value[2];




      
     while (tcounter) sleep_mode();
   
     tcounter = 14;//random(8 , 14);
     if (++counter2 == 0)  minutes--;



     
}
  for (int i=150; i >2; i--){
  {sleep_mode();OCR0B = i; OCR0A = i; OCR1B = i; }


  
    TCCR0A &= ~(1 << COM0B1); // disconnect pin
    TCCR0A &= ~(1 << COM0A1); // disconnect pin
    GTCCR &= ~(1 << COM1B1); // disconnect pin
    GTCCR &= ~(1 << COM1B0); // disconnect pin
    TCCR0B  &= ~(1 << CS01);   // disconnect clock
  PORTB &= ~(1 << LED);  // turn off LED
  PORTB &= ~(1 << LED0);  // turn off LED
  PORTB &= ~(1 << LED1);  // turn off LED
    MCUCR |= (1 << SM1); //set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Use the Power Down sleep mode
    WDTCR |= (1 << WDIE); // Enable watchdog timer interrupts
   minutes=0;offon=0; state=0; // night mode
  }
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
