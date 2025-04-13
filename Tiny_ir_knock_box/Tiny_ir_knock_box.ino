
// Устанавливаем максимальную частоту тактового сигнала
// CLKPR = 0x80;  // Включаем изменение предделителя тактового сигнала
// CLKPR = 0;     // Без предделителя (максимальная частота)

const int micPin = PB3;
const int lockPin = PB1;    // PB1 = Digital 9
const int tapperPin = PB0;  // PB13 = Digital 13

const int threshold = 400;     // Подбирается вручную под микрофон
const int tapDuration = 90;   // Длительность сигнала стука
const int debounceTime = 250;  // Минимальное время между стуками для защиты от эха

const int maxAttempts = 15;
int attemptCounter = 0;

const int patternLength = 6;
int wait_time[patternLength] = { 900, 1200, 500, 500, 500, 800 };
const int tolerance = 300;

unsigned long tapTimes[patternLength + 1];
int currentTap = 0;
unsigned long lastTapTime = 0;
bool listening = true;
unsigned long sound_time;
void setup() {

  // Устанавливаем максимальную частоту тактового сигнала
  CLKPR = 0x80;  // Включаем изменение предделителя тактового сигнала
  CLKPR = 0;     // Без предделителя (максимальная частота)

  pinMode(micPin, INPUT);
  pinMode(lockPin, OUTPUT);
  pinMode(tapperPin, OUTPUT);
  digitalWrite(lockPin, LOW);
    delay(1000);
    playCombination();
}

void loop() {

  if (millis() - sound_time > 85000) {
    playCombination();
    sound_time = millis();
  }

  if (listening) {
    if (detectTap()) {
      unsigned long now = millis();
      // Проверка анти-дребезга
      if (now - lastTapTime < debounceTime) {
        return;  // Игнорируем стук, слишком быстро
      }
       sound_time = millis();
        digitalWrite(tapperPin, HIGH);
        delay(100);
        digitalWrite(tapperPin, LOW);
      lastTapTime = now;
      tapTimes[currentTap] = now;
      if (currentTap == 0) {
        currentTap++;
      } else {
        int delta = tapTimes[currentTap] - tapTimes[currentTap - 1];
        int expected = wait_time[currentTap - 1];

        if (abs(delta - expected) <= tolerance) {
          currentTap++;

          if (currentTap > patternLength) {
            openLock();
            currentTap = 0;
          }
        } else {
          errorSignal();
          currentTap = 0;
        }
      }
     
    }
  }
}

bool detectTap() {
  int micValue = analogRead(micPin);
  if (micValue > threshold) {
  return true;
  }
  return false;
}

void openLock() {
  digitalWrite(lockPin, HIGH);
  delay(2000);
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
  for (int i = 0; i < patternLength+1; i++) {
    digitalWrite(tapperPin, HIGH);
    delay(tapDuration);
    digitalWrite(tapperPin, LOW);
    delay(wait_time[i]);
  }
}
