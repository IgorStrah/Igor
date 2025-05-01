#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#include <avr/power.h>
// Пины
#define LED_PIN 1  // Пин для управления WS2812
#define MIC_PIN 3  // Пин для цифрового микрофона (1 - шум, 0 - тишина)

// Константы
#define NUM_LEDS 4            // Количество светодиодов
#define LED_ALWAYS_ON 1       // Светодиод, который всегда горит (до выигрыша)
#define LED_NOISE 0           // Светодиод, реагирующий на шум
#define LED_SILENCE 2         // Светодиод, реагирующий на тишину
#define CYCLE_DURATION 3000   // Время на плавное загорание и затухание (в мс)
#define PAUSE_DURATION 500    // Пауза между циклами (в мс)
#define MIN_MIC_DURATION 400  // Минимальная продолжительность шума/тишины (в мс)
#define THRESHOLD 280         // Порог для микрофона


#define SAMPLE_INTERVAL 10  // Интервал между измерениями, в мс
#define BUFFER_SIZE 10      // Размер буфера для скользящего окна (50 * 10 мс = 500 мс)
// Константы для обработки сигнала
#define NOISE_THRESHOLD 550  // Минимальный уровень сигнала, который считается шумом (подбирается опытным путем)
#define MAX_SIGNAL 1023      // Максимальное значение для analogRead (10-битный ADC)

int micBuffer[BUFFER_SIZE];        // Буфер для хранения данных микрофона
int bufferIndex = 0;               // Текущий индекс буфера
int cloc_res = 0;                  // Текущий индекс буфера
unsigned long lastSampleTime = 0;  // Время последнего обновления фильтра

// Создаем объект Adafruit_NeoPixel
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Состояние игры
bool gameWon = false;                // Флаг завершения игры
unsigned long cycleStartMillis = 0;  // Время начала текущего цикла
unsigned long lastMicMillis = 0;     // Время последнего изменения состояния микрофона
bool noiseConfirmed = false;         // Было ли выполнено условие шума
bool silenceConfirmed = false;       // Было ли выполнено условие тишины
int correctCycles = 0;               // Счетчик правильных циклов
int currentLED = LED_NOISE;          // Текущий светодиод (0 или 2)

// Переменные для яркости
int brightness = 0;    // Текущая яркость светодиода
bool fadingUp = true;  // Флаг, указывающий, увеличиваем или уменьшаем яркость

void setup() {
  noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  pinMode(MIC_PIN, INPUT);
  strip.begin();
  strip.show();             // Очищаем светодиоды
  strip.setBrightness(50);  // Устанавливаем общую яркость

  // Заполняем буфер нулями (тишина)
  for (int i = 0; i < BUFFER_SIZE; i++) {
    micBuffer[i] = 0;
  }

  // Включаем "всегда горящий" светодиод (зеленый)
  strip.setPixelColor(LED_ALWAYS_ON, strip.Color(255, 0, 0));
  strip.show();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastSampleTime >= SAMPLE_INTERVAL) {
    lastSampleTime = currentMillis;

    // Считываем значение микрофона (аналоговый сигнал)
    int micValue = analogRead(MIC_PIN);

    // Конвертируем аналоговое значение в "шум или тишина" на основе порога
    micBuffer[bufferIndex] = (micValue > NOISE_THRESHOLD) ? 1 : 0;
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;  // Переход к следующему индексу
    int noiseLevel = calculateNoiseLevel();
  }


  if (gameWon) {
    // Если игра завершена, выключаем светодиод, который всегда горел
    strip.setPixelColor(LED_ALWAYS_ON, strip.Color(0, 0, 0));
    strip.show();
    return;
  }

  // Плавное загорание и затухание текущего светодиода
  if (currentMillis - cycleStartMillis < CYCLE_DURATION) {
    updateBrightness(currentMillis);
    if (brightness == 128) { checkMic(); }
    if (currentLED == 0) {
      strip.setPixelColor(currentLED, strip.Color(0, 0, brightness));  // Устанавливаем красный цвет
    } else {
      strip.setPixelColor(currentLED, strip.Color(brightness, 0, 0));  // Устанавливаем красный цвет
    }

    //  strip.setPixelColor(currentLED, strip.Color(brightness, 0, 0)); // Устанавливаем красный цвет
  } else if (currentMillis - cycleStartMillis < CYCLE_DURATION + PAUSE_DURATION) {
    // Пауза между циклами
    strip.setPixelColor(currentLED, strip.Color(0, 0, 0));  // Выключаем текущий светодиод
  } else {
    // Переход к следующему циклу
    cycleStartMillis = currentMillis;
    currentLED = (currentLED == LED_NOISE) ? LED_SILENCE : LED_NOISE;  // Переключаем светодиод

    if (cloc_res == 2) {
      noiseConfirmed = false;
      silenceConfirmed = false;
      cloc_res = 0;
    }
    cloc_res++;
  }
  // Победа
  if (correctCycles >= 5) {
    winGame();
  }

  // Обновляем светодиоды
  strip.show();
}

// Обновление яркости светодиода
void updateBrightness(unsigned long currentMillis) {
  unsigned long cycleProgress = currentMillis - cycleStartMillis;

  if (cycleProgress < CYCLE_DURATION / 2) {
    // Первая половина цикла: увеличение яркости
    brightness = map(cycleProgress, 0, CYCLE_DURATION / 2, 0, 255);
  } else {
    // Вторая половина цикла: уменьшение яркости
    brightness = map(cycleProgress, CYCLE_DURATION / 2, CYCLE_DURATION, 255, 0);
  }
}

// Проверка состояния микрофона
void checkMic() {



  int noiseLevel = calculateNoiseLevel();
  if (noiseLevel < 9 && currentLED == LED_NOISE) {
    // Проверка на шум в цикле LED_NOISE
    strip.setPixelColor(LED_ALWAYS_ON, strip.Color(100, 55, 0));
    noiseConfirmed = true;
  } else if (noiseLevel > 16 && currentLED == LED_SILENCE) {
    // Проверка на тишину в цикле LED_SILENCE
    strip.setPixelColor(LED_ALWAYS_ON, strip.Color(100, 55, 0));
    silenceConfirmed = true;
  } else {
    lastMicMillis = 0;  // Сбрасываем таймер, если условие не выполнено
    strip.setPixelColor(LED_ALWAYS_ON, strip.Color(255, 0, 0));
    correctCycles = 0;
    noiseConfirmed = false;
    silenceConfirmed = false;
  }
  strip.show();
  // Если оба условия выполнены, увеличиваем счетчик правильных циклов
  if (noiseConfirmed == true && silenceConfirmed == true) {
    correctCycles++;

    noiseConfirmed = false;
    silenceConfirmed = false;
  }
}

// Победа в игре
void winGame() {
  gameWon = true;
  strip.fill(strip.Color(0, 255, 255));  // Все светодиоды становятся зелеными
  strip.show();
  delay(2000);  // Пауза для отображения победы
  strip.clear();
  strip.show();

  enterDeepSleep();
}

// Функция для расчета уровня шума в процентах
int calculateNoiseLevel() {
  int sum = 0;

  // Суммируем все значения в буфере
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += micBuffer[i];
  }

  // Расчет процента: (количество "1") / (размер буфера) * 100
  return (sum * 100) / BUFFER_SIZE;
}

void enterDeepSleep() {
  // Отключаем ненужные модули для экономии энергии
  power_all_disable();

  // Устанавливаем режим сна "Power-down"
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Включаем режим сна
  sleep_enable();

  // Переходим в сон
  sleep_cpu();

  // После пробуждения (если оно произойдёт) выполнение начнётся отсюда, но для ATtiny85 это не нужно
  sleep_disable();
}