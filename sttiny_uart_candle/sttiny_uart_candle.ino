/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo and Micro support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>
#define LED PB1 // has to be PB1, since that pin is OC0B
#define LED1 PB0
#define LED2 PB0
#define GND PB2
SoftwareSerial mySerial(3, 4); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("Hello, world?");

}

void loop() { // run over and over

 long int j;

  pinMode(GND, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(GND, HIGH);
  digitalWrite(LED, LOW);
  
  pinMode(GND, INPUT);     // Переключаем 2й вывод на вход
  digitalWrite(GND, LOW);  // и отключаем на нем подтягивающий резистор



for ( j = 0; j < 500000; j++) {

 delay(1);

  if (bit_is_set(PINB, GND)==0) break;

  
  
}

 mySerial.println(j);

    

   delay(100);
 
}
