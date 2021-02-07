#include <Wire.h> 


// Set the LCD address to 0x27 for a 16 chars and 2 line display

int nano1=0;
int nano2;
int nano3;

void setup()
{
  Serial.begin(9600);
  // initialize the LCD
   Wire.begin(); 
}

void loop()
{


  Wire.requestFrom(2, 2);    // request 6 bytes from slave device #8
  int i=0;nano2=0;
  while (Wire.available()) { // slave may send less than requested
    byte c = Wire.read(); // receive a byte as character
    Serial.print(c);
    if (i==0) nano2 = ((c & 0xff) << 8); else nano2 = nano2 | c;
    i++;
  }
  Serial.println("Test send nano 2 ");
  Serial.println(nano2);

  delay(100);

  Wire.requestFrom(3, 2);    // request 6 bytes from slave device #8
  i=0;nano3=0;
  while (Wire.available()) { // slave may send less than requested
    byte c = Wire.read(); // receive a byte as character
    Serial.print(c);
    if (i==0) nano3 = ((c & 0xff) << 8); else nano3 = nano3 | c;
    i++;
  }

  
  nano1++;
  delay(800);
}
