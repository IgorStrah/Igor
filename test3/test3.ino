#define LED  PB1 // has to be PB1, since that pin is OC0B
#define LED0 PB4
#define LED1 PB0
#define KEY  PB2

void setup() {
  // put your setup code here, to run once:
   Serial.begin(57600);
  Serial.println("1Goodnight moon!");
  delay(100);

 DDRB |= (1 << LED) | (1 << LED0) | (1 << LED1)| (0 << KEY);

  
}

void loop() {
  // put your main code here, to run repeatedly:
 //   cli();
  PORTB |= (1 << LED) | (1 << LED0) | (1 << LED1);

   Serial.print("2Goodnight moon!  ");
   Serial.println(PINB & (1 << PINB3),BIN);
  Serial.print("1Goodnight moon!  ");
   Serial.println(bit_is_set(PINB,3),BIN);
  delay(100);
}
