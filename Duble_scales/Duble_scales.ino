/*!
 * @file readWeight.ino
 * @brief Get the weight of the object
 * @details After the program download is complete,
 * @n The serial port will print the current weight
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @License     The MIT License (MIT)
 * @author      [Wuxiao](xiao.wu@dfrobot.com)
 * @version  V1.0
 * @date  2020-12-26
 * @https://github.com/DFRobot/DFRobot_HX711
 */

#include <DFRobot_HX711.h>
int scales1, scales2;
/*!
 * @fn DFRobot_HX711
 * @brief Constructor 
 * @param pin_din  Analog data pins
 * @param pin_slk  Analog data pins
 */
DFRobot_HX711 MyScale1(6, 5);
DFRobot_HX711 MyScale2(7, 8);
int scale1, scale2;
void setup() {
  Serial.begin(115200);
    Serial.print("MS");
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
}

void loop() {
  scales1 = MyScale1.readWeight();
  scales2 = MyScale2.readWeight();
  // Get the weight of the object
  Serial.print("MyScale1 ");
  Serial.print(scales1, 1);
  Serial.println(" g");
  delay(200);
  Serial.print("MyScale2 ");
  Serial.print(scales2, 1);
  Serial.println(" g");
  delay(200);
  //310

  if ((scales1 > 290 && scales1 < 340)) {
    scale1++;
    if (scale1 > 3) {
      digitalWrite(A0, HIGH);
    }
  }

  if ((scales2 > 250 && scales2 < 280)) {

    scale2++;
    if (scale2 > 3) {
      digitalWrite(A1, HIGH);
    }
  }
}