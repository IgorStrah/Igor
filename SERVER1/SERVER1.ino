bool conditionsMet[5] = {false, false, false, false, false};

int locker=0;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
digitalWrite(2, HIGH);
digitalWrite(3, HIGH);
digitalWrite(4, HIGH);
digitalWrite(5, HIGH);
digitalWrite(6, HIGH);
digitalWrite(A0, HIGH);

 Serial.begin(9600);
delay(10000);
}

// the loop function runs over and over again forever
void loop() {

 
  if (digitalRead(7)==1){
  delay(100);
  if (digitalRead(7)==1)
  {digitalWrite(2, LOW);locker++; conditionsMet[0] = true;}
  }

  if (digitalRead(8)==1){
  delay(100);
  if (digitalRead(8)==1)
  {digitalWrite(3, LOW);locker++; conditionsMet[1] = true;}
  }

    if (digitalRead(9)==1){
  delay(200);
  if (digitalRead(9)==1)
  {digitalWrite(4, LOW);locker++; conditionsMet[2] = true;}
  }

    if (digitalRead(10)==1){
  delay(200);
  if (digitalRead(10)==1)
  {digitalWrite(5, LOW);locker++; conditionsMet[3] = true;}
  }

    if (digitalRead(11)==1){
  delay(100);
  if (digitalRead(11)==1)
  {digitalWrite(6, LOW);locker++; conditionsMet[4] = true;}
  }


bool allConditionsMet = true;
    for (int i = 0; i < 5; i++) {
        if (!conditionsMet[i]) {
            allConditionsMet = false;
            break;
        }
    }

    // Если все условия выполнены, срабатывает шестое
    if (allConditionsMet) {

  {digitalWrite(A0, LOW);}
  }
   
   
}
   

