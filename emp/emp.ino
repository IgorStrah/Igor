const int micPin = A0;
const int lockPin = 9;      // PB1 = Digital 9
const int tapperPin = 13;   // PB13 = Digital 13 (встроенный LED/stukalka)

const int threshold = 500;  // уровень для определения стука (подберём по опыту)
const int tapDuration = 150; // длина сигнала стука в мс

const int maxAttempts = 15;
int attemptCounter = 0;

const int patternLength = 6;
int wait_time[patternLength] = { 900, 1200, 500, 500, 500, 800 }; // эталонная пауза
const int tolerance = 300;

unsigned long tapTimes[patternLength + 1];
int currentTap = 0;
bool listening = true;

void setup() {
  pinMode(lockPin, OUTPUT);
  pinMode(tapperPin, OUTPUT);
  digitalWrite(lockPin, LOW); // замок закрыт
  Serial.begin(115200);
}

void loop() {
  if (listening) {
    if (detectTap()) {
      tapTimes[currentTap] = millis();
      if (currentTap == 0) {
        // Первый стук
        currentTap++;
      } else {
        // Проверяем паузу
        int delta = tapTimes[currentTap] - tapTimes[currentTap - 1];
        int expected = wait_time[currentTap - 1];

        if (abs(delta - expected) <= tolerance) {
          currentTap++;
          if (currentTap > patternLength) {
            // Комбинация введена верно
            openLock();
            currentTap = 0;
            attemptCounter = 0;
          }
        } else {
          // Ошибка — сброс
          attemptCounter++;
          errorSignal();
          currentTap = 0;

          if (attemptCounter >= maxAttempts) {
            playCombination();
            attemptCounter = 0;
          }
        }
      }

      // ждём, пока стук отпустят
      delay(200);
    }
  }
}

bool detectTap() {
  int micValue = analogRead(micPin);
  if (micValue > threshold) {
  
    Serial.print("   Tap detected!  ");
      Serial.println(micValue );
    return true;
  }
  return false;
}

void openLock() {
  digitalWrite(lockPin, HIGH);
  Serial.println("Unlocked!");
  delay(5000); // держим открытым 5 сек
  digitalWrite(lockPin, LOW);
}

void errorSignal() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(tapperPin, HIGH);
    delay(tapDuration);
    digitalWrite(tapperPin, LOW);
    delay(200);
  }
}

void playCombination() {
  for (int i = 0; i < patternLength; i++) {
    digitalWrite(tapperPin, HIGH);
    delay(tapDuration);
    digitalWrite(tapperPin, LOW);
    delay(wait_time[i]);
  }
}
