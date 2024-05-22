#include <Arduino.h>
#include <SoftwareSerial.h>
// Initialise on software serial port.
SoftwareSerial SoftSerial(2, 3);
#include "DYPlayerArduino.h"
DY::Player player(&SoftSerial);

void setup() {
  player.begin();
  // Also initiate the hardware serial port so we can use it for debug printing
  // to the console..
  Serial.begin(115200);
  player.setVolume(25); // 50% Volume
  //player.PlayMode(OneOff);//setCycleMode(DY::PlayMode::Repeat); // Play all and repeat.
  player.setCycleMode(DY::PlayMode::OneOff);
}

void loop() {




for (int i=0; i<=9; i++) {
 player.playSpecified(i);
  delay(2500);
}
 
 
}
