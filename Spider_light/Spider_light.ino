#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
const int ImputPin = PB3;  // Пин для активации
const int OutputPin = PB1;  // Пин для активации
unsigned long millis1 = 0; // Счётчик миллисекунд

    uint32_t lastTime = 0;     // Последнее время проверки
    uint32_t lastToggle = 0;   // Время последнего инвертирования
    uint16_t counter = 0;      // Счётчик секунд
    uint8_t state = 0;         // Состояние выхода (0 или 1)


void setup() {
  // put your setup code here, to run once:
noInterrupts();
  CLKPR = 0x80;  // enable clock prescale change
  CLKPR = 0;     // no prescale
  interrupts();
  // This example is for a 128x64 screen



    TCCR0A = (1 << WGM01);           // Режим CTC
    TCCR0B = (1 << CS01) | (1 << CS00); // Предделитель 64
    OCR0A = 124;                     // Прерывание каждую 1 мс при 8 МГц
    TIMSK = (1 << OCIE0A);           // Разрешаем прерывание по совпадению

    DDRB &= ~(1 << ImputPin);  // Настраиваем PB1 как вход
    DDRB |= (1 << OutputPin); // Настраиваем PB4 как выход
    PORTB|=(1 << OutputPin); // Устанавливаем PB4 в 

    sei();         // Включаем глобальные прерывания

  
}

ISR(TIM0_COMPA_vect) {
    millis1++; // Увеличиваем счётчик миллисекунд
}

void loop() {

        if (analogRead(ImputPin)>15) {
            // Если на входе 1, сбрасываем счётчик и выход
            counter = 0;
            PORTB |= (1 << OutputPin); // Устанавливаем PB4 в 1
            state = 0;
            lastTime = millis1; // Обновляем время, чтобы избежать лишних инверсий
        } else {
            // Если на входе 0 и прошло больше 1 секунды
            if (millis1 - lastTime >= 1000) {
                lastTime = millis1; // Обновляем время последней проверки
                counter++;         // Увеличиваем счётчик секунд

                if (counter <= 12) {
                    // Инвертируем состояние PB4 каждую секунду в течение 15 секунд
                    state ^= 1;
                    if (state) {
                        PORTB |= (1 << OutputPin);
                    } else {
                        PORTB &= ~(1 << OutputPin);
                    }
                } else {
                    // Если прошло 15 секунд, выставляем PB4 в 0
                    PORTB &= ~(1 << OutputPin);
                    enterDeepSleep();
                }
            }
        }
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
