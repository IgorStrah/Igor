int incomingByte = 0;
int servo=11;
 
int pos=0;
void setup() {
  
  pinMode(servo, OUTPUT);
}
 
void loop() {
  digitalWrite(servo, LOW);
pos=50;
      for (int i=1;i<=180;i++){        
        digitalWrite(servo, HIGH);
        delayMicroseconds(pos);
        digitalWrite(servo, LOW);
        delayMicroseconds(20000-pos);
        delay(1);
      }
   
    
  
}
