#include <IRremote.h>
#define BUTTON_A_PIN 0  // PB0
#define BUTTON_B_PIN 1  // PB1
#define BUTTON_C_PIN 2  // PB2
#define BUTTON_D_PIN 3  // PB3

#define SEQUENCE_LENGTH 3
#define COMBINATION_COUNT 4
#define TOTAL_BUTTONS 4
uint32_t currentIrCode = 0;
const uint8_t combinations[COMBINATION_COUNT][SEQUENCE_LENGTH] = {
  {BUTTON_A_PIN, BUTTON_C_PIN, BUTTON_B_PIN},
  {BUTTON_D_PIN, BUTTON_B_PIN, BUTTON_A_PIN},
  {BUTTON_B_PIN, BUTTON_D_PIN, BUTTON_C_PIN},
  {BUTTON_C_PIN, BUTTON_A_PIN, BUTTON_D_PIN}
};

uint8_t userSequence[SEQUENCE_LENGTH];
uint8_t currentStep = 0;

void setup() {
  noInterrupts();
  CLKPR = 0x80;
  CLKPR = 0;
  interrupts();
  IrSender.begin(4);
  pinMode(BUTTON_A_PIN, INPUT); // внешний pull-down
  pinMode(BUTTON_B_PIN, INPUT);
  pinMode(BUTTON_C_PIN, INPUT);
  pinMode(BUTTON_D_PIN, INPUT);
  IrSender.sendSonyMSB(1111000003, 32);
}

void loop() {
  uint8_t pressedButtons = getPressedButtons();

  // Если ничего не нажато — сброс
  if (pressedButtons == 0) {
    currentStep = 0;
    return;
  }

  switch (currentStep) {
    case 0: {
      int btn = getSinglePressedButton();
      if (btn != -1) {
        userSequence[0] = btn;
        currentStep = 1;
      }
      break;
    }

    case 1: {
      int secondBtn = getNewPressedButton(userSequence, 1);
      if (secondBtn != -1 && isButtonPressed(userSequence[0])) {
        userSequence[1] = secondBtn;
        currentStep = 2;
      } else if (!isButtonPressed(userSequence[0])) {
        currentStep = 0;
      }
      break;
    }

    case 2: {
      int thirdBtn = getNewPressedButton(userSequence, 2);
      if (thirdBtn != -1 &&
          isButtonPressed(userSequence[0]) &&
          isButtonPressed(userSequence[1])) {

        userSequence[2] = thirdBtn;

        int match = matchCombination(userSequence);
        if (match != -1) {
          triggerProcedure(match);
        }

        while (getPressedButtons() != 0); // ждём отпускания всех
        currentStep = 0;
      }
      break;
    }
  }
}

// Читает все кнопки в битовой маске
uint8_t getPressedButtons() {
  uint8_t state = 0;
  if (digitalRead(BUTTON_A_PIN) == HIGH) state |= (1 << 0);
  if (digitalRead(BUTTON_B_PIN) == HIGH) state |= (1 << 1);
  if (digitalRead(BUTTON_C_PIN) == HIGH) state |= (1 << 2);
  if (digitalRead(BUTTON_D_PIN) == HIGH) state |= (1 << 3);
  return state;
}

// Проверка, нажата ли конкретная кнопка
bool isButtonPressed(uint8_t pin) {
  return digitalRead(pin) == HIGH;
}

// Если нажата только одна кнопка — возвращает её
int getSinglePressedButton() {
  int count = 0;
  int last = -1;
  for (int pin = 0; pin < TOTAL_BUTTONS; pin++) {
    if (digitalRead(pin) == HIGH) {
      count++;
      last = pin;
    }
  }
  return (count == 1) ? last : -1;
}

// Возвращает кнопку, которая нажата и ещё не записана
int getNewPressedButton(uint8_t* existing, uint8_t count) {
  for (int pin = 0; pin < TOTAL_BUTTONS; pin++) {
    bool used = false;
    for (int i = 0; i < count; i++) {
      if (existing[i] == pin) {
        used = true;
        break;
      }
    }
    if (!used && digitalRead(pin) == HIGH) {
      return pin;
    }
  }
  return -1;
}

// Сравнивает введённую последовательность с эталонными
int matchCombination(uint8_t* sequence) {
  for (int i = 0; i < COMBINATION_COUNT; i++) {
    bool match = true;
    for (int j = 0; j < SEQUENCE_LENGTH; j++) {
      if (sequence[j] != combinations[i][j]) {
        match = false;
        break;
      }
    }
    if (match) return i;
  }
  return -1;
}

// Вызов действия
void triggerProcedure(int index) {
  switch (index) {
    case 0: procedure1(); break;
    case 1: procedure2(); break;
    case 2: procedure3(); break;
    case 3: procedure4(); break;
  }

  sendIrCodeMultipleTimes();  // Отправляем выбранный код 5 раз с паузой
}

// Процедуры теперь только задают нужный код
void procedure1() { currentIrCode = 1111000001; }
void procedure2() { currentIrCode = 1111000005; }
void procedure3() { currentIrCode = 1111000006; }
void procedure4() { currentIrCode = 1111000009; }  // исправил опечатку в числе

// Отправляем код 5 раз с паузой 500 мс
void sendIrCodeMultipleTimes() {
  for (int i = 0; i < 8; i++) {
    IrSender.sendSonyMSB(currentIrCode, 32);
    delay(500);  // Блокирующая задержка
  }
}
