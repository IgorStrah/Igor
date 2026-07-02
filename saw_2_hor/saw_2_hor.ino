#include <Arduino.h>

// ======================================================
// 4 кнопки A0..A3
// Нажимать 1->2->3->4, не отпуская предыдущие
//
// До успеха:
//   если отпустили уже "засчитанную" кнопку -> RESET
//
// Успех:
//   - дверь A5 = HIGH на 15 сек
//   - A4 = HIGH на 1 сек (один импульс)
//   - строб D9 мигает 15 сек (toggle раз в 500 мс)
//   - кнопки больше ни на что не влияют
//
// ДОП: пока работает стробоскоп (GS_RUN) пин 8 = LOW,
//      иначе пин 8 = HIGH.
// Без антидребезга. Всё в loop(). Только digitalWrite.
// ======================================================

// ---------- НАСТРОЙКА ----------
const bool BTN_ACTIVE_LOW = true;   // true = нажата = LOW

// ---------- ПИНЫ ----------
const uint8_t BTN1 = A0;
const uint8_t BTN2 = A1;
const uint8_t BTN3 = A2;
const uint8_t BTN4 = A3;

const uint8_t PULSE_A4 = A4;   // <<< ИМПУЛЬС 1 СЕК
const uint8_t DOOR     = A5;
const uint8_t STROBE   = 7;

const uint8_t L2 = 2;
const uint8_t L3 = 3;
const uint8_t L4 = 4;

const uint8_t PIN8_GND_WHEN_STROBE = 8;  // <<< НОВОЕ

// ---------- ТАЙМИНГИ ----------
const uint32_t RUN_MS = 22000;
const uint32_t STROBE_PERIOD_MS = 50;
const uint32_t PULSE_MS = 1000;

// ---------- СОСТОЯНИЯ ----------
const uint8_t GS_START = 0;
const uint8_t GS_S2    = 1;
const uint8_t GS_S3    = 2;
const uint8_t GS_S4    = 3;
const uint8_t GS_RUN   = 4;

uint8_t gs = GS_START; //!!!!!

// ---------- EDGE ----------
bool lastBtn[4] = {0,0,0,0};

// ---------- RUN таймеры ----------
uint32_t runStartMs = 0;
uint32_t lastStrobeMs = 0;
bool strobeLevel = false;

// ---------- A4 импульс ----------
bool pulseActive = false;
uint32_t pulseStartMs = 0;

// ---------- УТИЛИТА ----------
static inline bool isPressed(uint8_t pin) {
  bool lvl = digitalRead(pin);
  return BTN_ACTIVE_LOW ? (lvl == LOW) : (lvl == HIGH);
}

// ======================================================

void setup() {
    pinMode(PULSE_A4, OUTPUT);
     digitalWrite(PULSE_A4, LOW);
  Serial.begin(115200);

  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(BTN3, INPUT);
  pinMode(BTN4, INPUT);

  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  pinMode(L4, OUTPUT);
  pinMode(DOOR, OUTPUT);
  pinMode(STROBE, OUTPUT);
 

  pinMode(PIN8_GND_WHEN_STROBE, OUTPUT);          // <<< НОВОЕ

  digitalWrite(L2, LOW);
  digitalWrite(L3, LOW);
  digitalWrite(L4, LOW);
  digitalWrite(DOOR, LOW);
  digitalWrite(STROBE, LOW);
  digitalWrite(PULSE_A4, LOW);

  digitalWrite(PIN8_GND_WHEN_STROBE, HIGH);       // <<< НОВОЕ: по умолчанию к плюсу

  lastBtn[0] = isPressed(BTN1);
  lastBtn[1] = isPressed(BTN2);
  lastBtn[2] = isPressed(BTN3);
  lastBtn[3] = isPressed(BTN4);

  Serial.println("RESET -> START");
}

// ======================================================

void loop() {
  bool curBtn[4];
  bool pressEdge[4];
  bool releaseEdge[4];

  curBtn[0] = isPressed(BTN1);
  curBtn[1] = isPressed(BTN2);
  curBtn[2] = isPressed(BTN3);
  curBtn[3] = isPressed(BTN4);

  for (uint8_t i=0;i<4;i++) {
    pressEdge[i]   = ( curBtn[i] && !lastBtn[i]);
    releaseEdge[i] = (!curBtn[i] &&  lastBtn[i]);
  }

  uint32_t now = millis();

  // ===== A4 импульс =====
  if (pulseActive && (now - pulseStartMs >= PULSE_MS)) {
    digitalWrite(PULSE_A4, LOW);
    pulseActive = false;
    Serial.println("A4 PULSE OFF");
  }

  // ===== RUN режим =====
  if (gs == GS_RUN) {

    // <<< НОВОЕ: пока строб "работает" держим пин 8 в LOW
    digitalWrite(PIN8_GND_WHEN_STROBE, LOW);

    if (now - lastStrobeMs >= STROBE_PERIOD_MS) {

      lastStrobeMs = now;
      strobeLevel = !strobeLevel;
      digitalWrite(STROBE, strobeLevel ? HIGH : LOW);
    }

    if (now - runStartMs >= RUN_MS) {
      digitalWrite(DOOR, LOW);
      digitalWrite(STROBE, LOW);
      digitalWrite(L2, LOW);
      digitalWrite(L3, LOW);
      digitalWrite(L4, LOW);

      // <<< НОВОЕ: вне RUN пин 8 в HIGH
      digitalWrite(PIN8_GND_WHEN_STROBE, HIGH);

      strobeLevel = false;
      gs = GS_START;

      Serial.println("RUN END -> RESET");
    }

    for (uint8_t i=0;i<4;i++) lastBtn[i] = curBtn[i];
    return;
  }

  // <<< НОВОЕ: в любом другом режиме пин 8 должен быть в HIGH
  digitalWrite(PIN8_GND_WHEN_STROBE, HIGH);

  // ===== СБРОС ДО УСПЕХА =====
  uint8_t need = 0;
  if (gs == GS_S2) need = 1;
  else if (gs == GS_S3) need = 2;
  else if (gs == GS_S4) need = 3;

  for (uint8_t i=0;i<need;i++) {
    if (releaseEdge[i]) {
      gs = GS_START;
      digitalWrite(L2, LOW);
      digitalWrite(L3, LOW);
      digitalWrite(L4, LOW);
      Serial.println("RELEASE REQUIRED -> RESET");
      for (uint8_t k=0;k<4;k++) lastBtn[k] = curBtn[k];
      return;
    }
  }

  // ===== ШАГИ =====
  if (gs == GS_START) {
    if (pressEdge[0]) {
      gs = GS_S2;
      digitalWrite(L2, HIGH);
      Serial.println("STEP1 OK");
    }
  }
  else if (gs == GS_S2) {
    if (pressEdge[1] && curBtn[0]) {
      gs = GS_S3;
      digitalWrite(L3, HIGH);
      Serial.println("STEP2 OK");
    }
  }
  else if (gs == GS_S3) {
    if (pressEdge[2] && curBtn[0] && curBtn[1]) {
      gs = GS_S4;
      digitalWrite(L4, HIGH);
      Serial.println("STEP3 OK");
    }
  }
  else if (gs == GS_S4) {
    if (pressEdge[3] && curBtn[0] && curBtn[1] && curBtn[2]) {
      // ===== SUCCESS =====
      gs = GS_RUN;
      runStartMs = now;
      lastStrobeMs = now;
      strobeLevel = false;

      digitalWrite(STROBE, LOW);

      // ---- A4 импульс ----
      digitalWrite(PULSE_A4, HIGH);
        delay(2000);
      digitalWrite(DOOR, HIGH);
      pulseStartMs = now;
      pulseActive = true;
      Serial.println("A4 PULSE ON");

      Serial.println("SUCCESS -> RUN");
    }
  }

  for (uint8_t i=0;i<4;i++) lastBtn[i] = curBtn[i];
}
