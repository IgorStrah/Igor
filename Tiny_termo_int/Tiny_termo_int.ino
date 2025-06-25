/* 
 * Trinket / ATtiny85 Temperature and Voltage Measurement
 * Adapted from
 * http://forum.arduino.cc/index.php/topic,26299.0.html,
 * http://www.mikrocontroller.net/topic/315667, and
 * http://goetzes.gmxhome.de/FOSDEM-85.pdf
*/
// Values will be output to Trinket pin #0 via software serial for this example.  
// The library SendOnlySoftwareSerial by Nick Gammon is used to save a pin over
// using the SoftwareSerial library 
// http://gammon.com.au/Arduino/SendOnlySoftwareSerial.zip

#include <SoftwareSerial.h>

// TX = PB1 (pin 6), RX не используется
SoftwareSerial mySerial(0, 1); 

void setup() {
    noInterrupts();
  CLKPR = 0x80;  // enable change
  CLKPR = 0x00;  // no prescaler
  interrupts();
  // Setup the Analog to Digital Converter -  one ADC conversion
  //   is read and discarded
  ADCSRA &= ~(_BV(ADATE) |_BV(ADIE)); // Clear auto trigger and interrupt enable
  ADCSRA |= _BV(ADEN);                // Enable AD and start conversion
  ADMUX = 0xF | _BV( REFS1 );         // ADC4 (Temp Sensor) and Ref voltage = 1.1V;
  delay(100);                         // Settling time min 1 ms, take 100 ms
  getADC();

  mySerial.begin(9600);                 // set up serial output
}

void loop() {
  int i;
  int t_celsius; 
  uint8_t vccIndex;
  float rawTemp, rawVcc;
  
  // Measure temperature
  ADCSRA |= _BV(ADEN);           // Enable AD and start conversion
  ADMUX = 0xF | _BV( REFS1 );    // ADC4 (Temp Sensor) and Ref voltage = 1.1V;
  delay(100);                    // Settling time min 1 ms, wait 100 ms

  rawTemp = (float)getADC();     // use next sample as initial average
  for (int i=2; i<2000; i++) {   // calculate running average for 2000 measurements
    rawTemp += ((float)getADC() - rawTemp) / float(i); 
  }  
  ADCSRA &= ~(_BV(ADEN));        // disable ADC  

  // Measure chip voltage (Vcc)
  ADCSRA |= _BV(ADEN);   // Enable ADC
  ADMUX  = 0x0c | _BV(REFS2);    // Use Vcc as voltage reference,
                                 //    bandgap reference as ADC input
  delay(100);                    // Settling time min 1 ms, there is
                                 //    time so wait 100 ms
  rawVcc = (float)getADC();      // use next sample as initial average
  for (int i=2; i<2000; i++) {   // calculate running average for 2000 measurements
    rawVcc += ((float)getADC() - rawVcc) / float(i);
  }
  ADCSRA &= ~(_BV(ADEN));        // disable ADC
  
  rawVcc = 1024 * 1.1f / rawVcc;
  //index 0..13 for vcc 1.7 ... 3.0
  vccIndex = min(max(17,(uint8_t)(rawVcc * 10)),30) - 17;   

  // Temperature compensation using the chip voltage 
  // with 3.0 V VCC is 1 lower than measured with 1.7 V VCC 
  t_celsius = (int)(chipTemp(rawTemp) + (float)vccIndex / 13);  
                                                                                   
  mySerial.print("Temp: ");
  mySerial.println(t_celsius);
  mySerial.print("Vcc: ");
  mySerial.println(rawVcc);
}

// Calibration of the temperature sensor has to be changed for your own ATtiny85
// per tech note: http://www.atmel.com/Images/doc8108.pdf
float chipTemp(float raw) {
  const float chipTempOffset = 223;           // Your value here, it may vary 
  const float chipTempCoeff = 1.1;            // Your value here, it may vary
  return((raw - chipTempOffset) / chipTempCoeff);
}
 
// Common code for both sources of an ADC conversion
int getADC() {
  ADCSRA  |=_BV(ADSC);           // Start conversion
  while((ADCSRA & _BV(ADSC)));    // Wait until conversion is finished
  return ADC;
}