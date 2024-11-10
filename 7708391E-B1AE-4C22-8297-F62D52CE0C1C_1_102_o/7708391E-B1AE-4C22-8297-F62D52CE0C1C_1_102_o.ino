
void setup() {
  DDRC |= _BV(DDC4);  // Pin B1 - Dig pin9. смотришь на схеме что написано типа PB1 -> DDB1(9),  PC0 -> DDC0(A0)
  // сейчас стоит А4. DDC4
  
}

void loop() {
  setServoPosition(700);  // 1 мс - 0 граду. НО! погрешность! поэтому подбираем руками (700-1200)
  delay(1000);  
  setServoPosition(2400);  // 20 мс - 180 НО! читай выше (200)
  delay(1000); 
}
// Если будет глючить по чтению карточки или пултта нужно поставить перед вызовом этой функции
// cli() - выключить все прерывания и не откликаться на них и после сервы -  sei() включить обратно все прерывания. 
// все принятые данные за этот промежуток будут утеряны.

void setServoPosition(int pulseWidth) {
  PORTC |= _BV(PORTC4); // analod digitalwrite. bet faster 
  preciseDelayMicroseconds(pulseWidth);  
  PORTC &= ~_BV(PORTC4);  
  delay(20 - pulseWidth / 1000); // ±20 ms
}

// кусок магии.  "nop\n\t" - выполняет "ничего" 1 такт процессора.
// 10 циклов ничего + 4-5 тактов на сравнение  и операцию вычитания и получаем 15 тактов
// за секунду у нас 16 000 000, тоесть задержка чуууу чуууть меньше 1 микросекунды учитывая погрешность

void preciseDelayMicroseconds(int us) {
  while (us--) {
    // вичитание занимает примерно 4-5 такта на каждый цикл
    asm volatile(
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
    );
  }
}
