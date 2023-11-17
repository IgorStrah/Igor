//--------------------------------------------
// Displaying RGB Colors on WS2812B LED Strip
// using WS2812B_LED_Strip.h Library
// by: Anas Kuzechie (June, 2022)
//--------------------------------------------
#include <WS2812B_LED_Strip.h>
//--------------------------------------------
WS2812B_LED_Strip WS2812B(2); /*DIN can only be connected to D2-->D7*/
//========================================================================
void setup()
{
  WS2812B.clearLEDs(30, 1);   /*turn OFF all LEDs & reset WS2812B*/
  delay(500);
}
//========================================================================
void loop()
{
  /*bargraph pattern - increasing & decreasing*/
  byte numLEDs = 10;
  for(byte i=1; i<=numLEDs; i++)
  {
    for(byte j=1; j<=i; j++) WS2812B.color(0, 0, 255/16);
    delay(200);
  }
  //----------------------------------------------------------
  delay(500);
  //----------------------------------------------------------
  for(byte i=numLEDs-1; i>=1; i--)
  {
    WS2812B.clearLEDs(i+1, 1);
    for(byte j=1; j<=i; j++) WS2812B.color(0, 0, 255/16);
    delay(200);
  }
  WS2812B.clearLEDs(1, 1);
  delay(500);
}