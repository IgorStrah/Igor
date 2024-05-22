// Пины кнопок
const int buttonPins[] = { 2, 3, 4, 5, 6 };
// Пин открытия замка
const int lockPin = 8;
// Правильная последовательность нажатий на кнопки
const int correctCode[] = { 0, 2, 4, 3, 1, 3 };
const int codeLength = 6;  // Длина кода
int Code[] = { 0, 0, 0, 0, 0, 0 };
int buttonState[5] = {};
int previousButtonState;
int currentIndex = 0;  // Текущий индекс для добавления нового значения


void setup() {
  // Настройка пинов кнопок
  for (int i = 0; i < 5; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Настройка пина открытия замка
  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, LOW);
  // Настройка последовательного порта
  Serial.begin(115200);
  Serial.println("Кодовый замок инициализирован.");
}

void loop() {
  int enteredCode[codeLength];
  int enteredCodeIndex = 0;

  // Чтение состояния кнопок и запись нажатых кнопок в массив
  for (int i = 0; i < 5; i++) {
    buttonState[i] = digitalRead(buttonPins[i]);
    if (buttonState[i] == HIGH && previousButtonState != i) {
      enteredCode[enteredCodeIndex] = i;
      enteredCodeIndex++;
      addData(i);
      // Serial.print("previousButtonState[i] != buttonState[i]  ");
      // Serial.print(buttonState[i]);
      // Serial.print("    ");
      // Serial.print(previousButtonState);
      // Serial.print("    ");
      // Serial.println(i);
      delay(50);  // Для устранения дребезга
      previousButtonState = i;
      printArray();
    }
  }


  bool codeCorrect = true;
  for (int i = 0; i < codeLength; i++) {
    // Serial.print(" Code[i]   ");
    // Serial.print(Code[i]);
    // Serial.print(" correctCode[i]   ");
    // Serial.print(correctCode[i]);

    if (Code[i] != correctCode[i]) {
      codeCorrect = false;
      break;
    }
  }
  // Serial.println();
  // Если код правильный, открыть замок
  if (codeCorrect) {
    unlock();
    delay(1000);  // Подать импульс на 1 секунду
    Serial.println("Замок открыт.");
    for (int i = 0; i < codeLength; i++) {
      Code[i - 1] = 0;
    }
  }
}

void addData(int data) {

  // Если массив полностью заполнен, сдвигаем значения
  for (int i = 0; i < codeLength; i++) {
    Code[i - 1] = Code[i];
  }
  Code[codeLength - 1] = data;  // Добавляем новое значение в конец массива
}
void printArray() {
  for (int i = 0; i < codeLength; i++) {

    Serial.print("   i");
    Serial.print(i);
    Serial.print("  ");
    Serial.print(Code[i]);
  }
  Serial.println();
}


void unlock() {
  digitalWrite(lockPin, HIGH);  // Подать импульс на пин открытия замка
}
