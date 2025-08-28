#include <Arduino.h>
#include <math.h>
#include <arduinoFFT.h>

// Pitch meter + note logger — ArduinoFFT v2.x (camelCase), N=128, Fs≈8 kHz
// A0 — микрофон (AC, смещение к ~Vcc/2). Serial Monitor @115200

#define MIC_PIN A0

// --- калибровка дискретизации: подай 1000 Гц и подстрой до ~1000.0 ---
const float FS_CAL = 1.000f;  // пример: 1.020f = +2%

// --- FFT / sampling ---
const uint16_t NUM_SAMPLES        = 128;
const uint16_t SAMPLING_FREQUENCY = (uint16_t)(8000 * FS_CAL);
const uint32_t SAMPLE_PERIOD_US   = 1000000UL / SAMPLING_FREQUENCY;

// --- диапазон поиска и чувствительность ---
const float FREQ_MIN = 600.0f;     // расширили вниз
const float FREQ_MAX = 3800.0f;    // чуть ниже Найквиста (fs/2) с запасом
const float PEAK_OVER_MEAN = 5.5f; // ЧУВСТВИТЕЛЬНЕЕ (было 8.0)

// --- лента нот ---
const uint16_t NOTE_DEBOUNCE_MS = 40;
static const float NOTE_HYST_CENTS = 25.0f;  // гистерезис между полутонами

// --- удержание тона (не рвём короткие провалы) ---
const uint16_t HOLD_GAP_MS = 60;   // если тишина ≤ 60 мс — держим прошлую ноту

// буферы FFT
float vReal[NUM_SAMPLES];
float vImag[NUM_SAMPLES];
ArduinoFFT<float> FFT(vReal, vImag, NUM_SAMPLES, SAMPLING_FREQUENCY);

// прототипы
int   freqToMidi(float f);
const char* name12(int pc);
void  printPitchLine(float f);

// лог удерживаемой ноты
struct HeldNote {
  int lastMidi;
  unsigned long tStart;
} hold = {-1, 0};

void setup() {
  Serial.begin(115200);
  analogReference(DEFAULT);
  pinMode(MIC_PIN, INPUT);
  Serial.println(F("Pitch meter (FFT v2.x, DC-remove, 0.6–3.8 kHz, N=128, sensitive)"));
}

void loop() {
  // --- сбор выборок + реальное среднее (DC-блок) ---
  uint32_t t = micros();
  uint32_t acc = 0;
  for (uint16_t i = 0; i < NUM_SAMPLES; i++) {
    int16_t s = analogRead(MIC_PIN);
    acc += s;
    vReal[i] = (float)s;
    vImag[i] = 0.0f;

    t += SAMPLE_PERIOD_US;
    while ((int32_t)(t - micros()) > 0) { /* wait */ }
  }
  float dc = (float)acc / (float)NUM_SAMPLES;
  for (uint16_t i = 0; i < NUM_SAMPLES; i++) vReal[i] -= dc;

  // --- FFT ---
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();

  // --- поиск пика в заданном диапазоне (без min/max шаблонов) ---
  uint16_t kMin = (uint16_t)ceilf(FREQ_MIN * NUM_SAMPLES / SAMPLING_FREQUENCY);
  if (kMin < 1) kMin = 1;
  uint16_t kMax = (uint16_t)floorf(FREQ_MAX * NUM_SAMPLES / SAMPLING_FREQUENCY);
  uint16_t nyq  = (NUM_SAMPLES / 2) - 1;
  if (kMax > nyq) kMax = nyq;

  float peak = 0.0f, sum = 0.0f;
  uint16_t kPeak = 0, cnt = 0;
  for (uint16_t k = kMin; k <= kMax; k++) {
    float m = vReal[k];
    sum += m; cnt++;
    if (m > peak) { peak = m; kPeak = k; }
  }

  static float fEMA = 0.0f;                  // EMA сглаживание частоты
  static unsigned long lastGoodMs = 0;       // когда последний раз был хороший тон
  float freq = 0.0f;

  if (kPeak != 0 && cnt != 0) {
    float mean = sum / (float)cnt;
    if (mean > 0.0f && (peak / mean) >= PEAK_OVER_MEAN) {
      // лог-параболическая интерполяция пика
      float kInterp = (float)kPeak;
      if (kPeak > kMin && kPeak < kMax) {
        float y1 = logf(vReal[kPeak - 1] + 1e-6f);
        float y2 = logf(vReal[kPeak]     + 1e-6f);
        float y3 = logf(vReal[kPeak + 1] + 1e-6f);
        float denom = 2.0f * y2 - y1 - y3;
        if (fabsf(denom) > 1e-6f) {
          float delta = 0.5f * (y1 - y3) / denom;
          kInterp = (float)kPeak + delta;
        }
      }
      freq = kInterp * ((float)SAMPLING_FREQUENCY / (float)NUM_SAMPLES);

      // EMA для устойчивых центов
      fEMA = (fEMA == 0.0f) ? freq : (0.8f * fEMA + 0.2f * freq);
      freq = fEMA;

      lastGoodMs = millis();
    }
  }

  // --- grace gap: короткая тишина — держим последнюю частоту ---
  if (freq <= 0.0f) {
    if (fEMA > 0.0f && (millis() - lastGoodMs) <= HOLD_GAP_MS) {
      freq = fEMA;  // удерживаем
    }
  }

  // --- печать «частота → нота» ---
  printPitchLine(freq);

  // --- лента нот с гистерезисом по высоте ---
  int midi = (freq > 0.0f)
             ? (int)lroundf(69.0f + 12.0f * (logf(freq/440.0f)/logf(2.0f)))
             : -1;

  static int midiHold = -1;
  int midiOut = -1;

  if (midi >= 0) {
    if (midiHold < 0) midiHold = midi;
    float nFloat = 69.0f + 12.0f * (logf(freq/440.0f)/logf(2.0f));
    float centsFromHold = (nFloat - (float)midiHold) * 100.0f;
    if (fabsf(centsFromHold) > NOTE_HYST_CENTS) midiHold = midi;
    midiOut = midiHold;
  } else {
    midiOut = -1;
    midiHold = -1;
  }

  unsigned long now = millis();
  static int lastMidiForLog = -1;
  static unsigned long lastStart = 0;

  if (midiOut >= 0) {
    if (lastMidiForLog < 0) {
      lastMidiForLog = midiOut;
      lastStart = now;
    } else if (midiOut != lastMidiForLog) {
      unsigned long dur = now - lastStart;
      if (dur >= NOTE_DEBOUNCE_MS) {
        int pc = (lastMidiForLog % 12 + 12) % 12;
        int oct = lastMidiForLog / 12 - 1;
        Serial.print(F("[NOTE] "));
        Serial.print(name12(pc)); Serial.print(oct);
        Serial.print(' '); Serial.print(dur); Serial.println(F(" ms"));
      }
      lastMidiForLog = midiOut;
      lastStart = now;
    }
  } else {
    if (lastMidiForLog >= 0) {
      unsigned long dur = now - lastStart;
      if (dur >= NOTE_DEBOUNCE_MS) {
        int pc = (lastMidiForLog % 12 + 12) % 12;
        int oct = lastMidiForLog / 12 - 1;
        Serial.print(F("[NOTE] "));
        Serial.print(name12(pc)); Serial.print(oct);
        Serial.print(' '); Serial.print(dur); Serial.println(F(" ms"));
      }
      lastMidiForLog = -1;
    }
  }
}

// -------------- helpers --------------
int freqToMidi(float f) {
  if (f <= 0.0f) return -1;
  float n = 69.0f + 12.0f * (logf(f / 440.0f) / logf(2.0f)); // 69 = A4
  return (int)lroundf(n);
}

const char* name12(int pc) {
  static const char* n[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
  return n[(pc % 12 + 12) % 12];
}

void printPitchLine(float f) {
  if (f <= 0.0f) { Serial.println(F("—")); return; }
  float nFloat = 69.0f + 12.0f * (logf(f / 440.0f) / logf(2.0f));
  int   nRound = (int)lroundf(nFloat);
  int   pc     = (nRound % 12 + 12) % 12;
  int   oct    = nRound / 12 - 1;
  float cents  = (nFloat - (float)nRound) * 100.0f;

  Serial.print(f, 1); Serial.print(F(" Hz  →  "));
  Serial.print(name12(pc)); Serial.print(oct);
  Serial.print(F("  ("));
  if (cents >= 0) Serial.print('+');
  Serial.print(cents, 1); Serial.println(F(" cents)"));
}
