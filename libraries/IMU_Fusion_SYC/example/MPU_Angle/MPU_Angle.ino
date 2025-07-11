/********************************************************
  @author: Vegetable_SYC
  @address: https://github.com/Vegetable-SYC/IMU_Fusion_SYC
  @version: v1.2.1
  
  @note:
   Please note that the automatic calibration procedure needs
   to be executed after the MPU6050 is initialized, and the 
   execution order cannot be changed, otherwise an error may
   occur!!!!
   
   If you find that some boards are not working, 
   you can contact me on GitHub or send me an email at 
   1318270340@qq.com.
 *******************************************************/

#include "IMU_Fusion_SYC.h"

IMU imu(Wire);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  imu.begin(CHOOSE_MPU6050);             // Select MPU6050
  // imu.MPU6050_SetGyroOffsets(0, 0, 0);// MPU6050 manually calibrated
  imu.MPU6050_CalcGyroOffsets();         // MPU6050 automatic calibration
}

void loop() {
  imu.Calculate();                       // Calculating Angle
  Serial.print("\nangleX : ");
  Serial.print(imu.getAngleX());
  Serial.print("\tangleY : ");
  Serial.print(imu.getAngleY());
  Serial.print("\tangleZ : ");
  Serial.println(imu.getAngleZ());
}