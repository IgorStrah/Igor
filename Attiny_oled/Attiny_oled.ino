#include <QMC5883LCompass.h>

QMC5883LCompass compass;

#include <Tiny4kOLED.h>
// ============================================================================

void setup() {
  // put your setup code here, to run once:
noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  // This example is for a 128x64 screen
  oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
  compass.init();
  compass.setCalibrationOffsets(-60.00, 1672.00, 138.00);
compass.setCalibrationScales(1.02, 0.93, 1.06);
  // Two rotations are supported,
  // The begin() method sets the rotation to 1.
  //oled.setRotation(0);

  // Some newer devices do not contain an external current reference.
  // Older devices may also support using the internal curret reference,
  // which provides more consistent brightness across devices.
  // The internal current reference can be configured as either low current, or high current.
  // Using true as the parameter value choses the high current internal current reference,
  // resulting in a brighter display, and a more effective contrast setting.
  //oled.setInternalIref(true);

  oled.clear();

  oled.on();
}

void loop() {
  int a;
    // Read compass values
  compass.read();

  // Return Azimuth reading
  a = compass.getAzimuth();
  

   oled.setFont(FONT8X16P);
  oled.setCursor(0, 1);
  oled.print("A:");
  oled.setFont(FONT8X16P);
  oled.setCursor(25, 1);
  oled.print("         ");

  oled.setFont(FONT8X16P);
  oled.setCursor(24, 1);
  oled.print(a);


  delay(1000);



}




