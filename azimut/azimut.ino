
#include <DFRobot_QMC5883.h>
#include <Wire.h>

#include <Tiny4kOLED.h>
DFRobot_QMC5883 compass(&Wire, /*I2C addr*/ HMC5883L_ADDRESS);

void setup() {

  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();

  oled.begin(128, 32, sizeof(tiny4koled_init_128x32br), tiny4koled_init_128x32br);
  oled.on();
  oled.clear();

  oled.setFont(FONT8X16P);
  oled.setCursor(0, 1);
  oled.print("A:");
  oled.setFont(FONT8X16P);
  oled.setCursor(25, 1);
  oled.print("         ");

  oled.setFont(FONT8X16P);
  oled.setCursor(24, 1);
  oled.print("START");
  delay(1500);

  while (!compass.begin()) {
    oled.setFont(FONT8X16P);
    oled.setCursor(0, 1);
    oled.print("A:");
    oled.setFont(FONT8X16P);
    oled.setCursor(25, 1);
    oled.print("         ");

    oled.setFont(FONT8X16P);
    oled.setCursor(24, 1);
    oled.print("FAIL");
    delay(500);
  }
}
void loop() {
  /**
   * @brief  Set declination angle on your location and fix heading
   * @n      You can find your declination on: http://magnetic-declination.com/
   * @n      (+) Positive or (-) for negative
   * @n      For Bytom / Poland declination angle is 4'26E (positive)
   * @n      Formula: (deg + (min / 60.0)) / (180 / PI);
   */
  float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / PI);
  compass.setDeclinationAngle(declinationAngle);
  sVector_t mag = compass.readRaw();
  compass.getHeadingDegrees();

  //azimuth = (int)(atan2((float)mag.YAxis, (float)mag.XAxis) * 180.0 / 3.14159); // Азимут в градусах

  int azimuth = atan2(mag.XAxis, mag.YAxis) * 180.0 / PI;  // Азимут в градусах

  // Приведение к диапазону 0–360
  if (azimuth < 0) {
    azimuth += 360.0;
  }


  oled.setFont(FONT8X16P);
  oled.setCursor(0, 1);
  oled.print("x:");
  oled.setFont(FONT8X16P);
  oled.setCursor(25, 1);
  oled.print("         ");

  oled.setFont(FONT8X16P);
  oled.setCursor(24, 1);
  oled.print(azimuth);



  // Serial.print("X:");
  // Serial.print(mag.XAxis);
  // Serial.print(" Y:");
  // Serial.print(mag.YAxis);
  // Serial.print(" Z:");
  // Serial.println(mag.ZAxis);
  // Serial.print("Degress = ");
  // Serial.println(mag.HeadingDegress);
  delay(200);
}
