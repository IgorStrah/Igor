/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int RECV_PIN = 3;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  irrecv.enableIRIn(); // Start the receiver
   DDRB  |=  (1<<1)|(1<<2); // Устанавливаем пин D13 в режим OUTPUT
}

void loop() {

       //PORTB |= (1 << 2);
       // delay (1000);
       // PORTB &= ~ (1 << 2);
       // delay (1000);

  
  if (irrecv.decode(&results)) {
     irrecv.resume(); // Receive the next value
    delay(100);
      if ((results.value == 16726215) || (results.value == 1111000004)) {
        
        PORTB |= (1 << 1);
        delay (70000);
        PORTB &= ~ (1 << 1);
        delay (1000);
  
  }
  delay(100);
}
 delay(100);
}
