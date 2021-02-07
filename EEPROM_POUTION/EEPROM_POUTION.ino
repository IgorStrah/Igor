/*
 * EEPROM Write
 *
 * Stores values read from analog input 0 into the EEPROM.
 * These values will stay in the EEPROM when the board is
 * turned off and may be retrieved later by another sketch.
 */

#include <EEPROM.h>



 const char  *potions[] =
{
"4939DF2A96A80",//   икра бакложанно забы сухолапки
"40E9DF2A96A80",//   костная мука
"4219EF2A96A80",//   собачий корень
"4099EF2A96A80",//   тушёная мандрагора
"49F9EF2A96A80",//   Сок мурлакомля
"4199EF2A96A80",//   перья выскакунчика
"4C39FF2A96A80",//   лиловый горноцвет
"4749FF2A96A81",//   стандартный ингридиент Н
"4709FF2A96A81",//   Жир Тролля  
"4439DF2A96A80",//   Шерсть с ног акрамантула
"40D9EF2A96A80",//   Чешуя рыбы убийцы
"40A9DF2A96A80",//   челюсти пучеглазки
"4E29EF2A96A80",//   Лапка детёныша каппы
"4139FF2A96A81",//   Крошечные глаза жуков
"49F9DF2A96A80",//   Икра жёлтой жижаглазки
"45B9DF2A96A80",//   Икра оранживо скряги
"48F9DF2A96A80",//   Кусачи кракл
"45F9DF2A96A80",//   Подземны чекатун
"4849FF2A96A81",//   Морские жёлуди
"45E9EF2A96A81",//   драконья кровь
"4129DF2A96A80",//   Жаренная кожа крысы
"45A9EF2A96A81",//   Поджаренные стрекозиные грудки
"4F79FF2A96A80",//   Эмбрион саламандры
"4EF9FF2A96A80",//   когти грифона
"4539DF2A96A80",//   Икра зелёной жабы рогатки
"4AB9DF2A96A80",//   Икра Эфирного Пологрота
"4AF9DF2A96A80",//   Икра сини Нифоганки
"4FE9EF2A96A80",//   Дождевые черви
"4FB9FF2A96A80",//   Вытяжка зародыше апаллала
"49B9DF2A96A80",//   Жук погонщик
"4979DF2A96A80",//   Жукаморфный трикоид
"4A39DF2A96A80",//   Кальтучи ступляк
"4909FF2A96A81",//   Лунны колотень
"40F9FF2A96A81",//   Слизь мозга линивца
"4579DF2A96A80",//   Чудоковатый тапатун
"43D9DF2A96A81",//   Нашинкованные мёртвые гусенницы
"4EE9EF2A96A80",//   Шкура саламандры
"4639DF2A96A80",//   Икра красно жабы людоеда
"4469EF2A96A81",//   Волосы ламии
"4629EF2A96A81",//   Сопли тролля
"4029EF2A96A81",//   Шкура экроцирпуса
"4FF9FF2A96A80",//   Слизь Флоббер-Червя
"4079FF2A96A81",//   Слизб болотника
"4B39DF2A96A80",//   яд василиска
"40B9FF2A96A81",//   кровь саламандры
"44F9DF2A96A80",//   Плоть мертвеца
"41D9EF2A96A80",//   Сок пиявки
"4BF9FF2A96A80",//   Порошок рога единорога
"4F69EF2A96A80",//   Лунны осминог
"4119EF2A96A80",//   Сок ягоды БУМ!
"4479DF2A96A80",//   Яйца докси
"44B9DF2A96A80",//   травной стручёк
"48E9EF2A96A81",//   Змеиные Клыки
"44E9EF2A96A81",//   Капли мёртвой воды 
"4F29EF2A96A80",//   Спиртовая настойка
"43F9DF2A96A80",//   Паучье яйцо
"4569EF2A96A81",//   Иглы дикообраза
"4A49EF2A96A80",//   Мёртвая кровь
"4729EF2A96A80",//   Кровь великана
"4EA9EF2A96A80",//   Бесовски гриб
"45A9EF2A96A80",//   Дыхание амистра
"44E9EF2A96A80",//   Ложеница
"46E9EF2A96A80",//   Сепосфера
"4359DF2A96A81",//   Корни лазовика
"48C9FF2A96A81",//   Глаз Слепого Кота
"4499DF2A96A81",//   Прах вампира
"4B89EF2A96A80",//   Ядовитый Колокольчик
"4A89EF2A96A80",//   Лёд со дна серебристого озера
"4AC9EF2A96A80",//   Хребты Рыбы-Льва
"4039FF2A96A81",//   Безумные Многоножки
"47C9FF2A96A81",//   Экстракт Фенхеля
"4B49EF2A96A80",//   Огненная Соль
"4889FF2A96A81",//   Крысиная селезёнка
"4419DF2A96A81",//   Сушёная крапива
"4319DF2A96A81",//   Порошок из костей фестрала
"4DE9EF2A96A80",//   Жилы кентавра
"46A9EF2A96A80",//     Лоскут одежды Баньши


  
};







/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
int addr = 0;

void setup() {
  /** Empty setup. **/

  Serial.begin(115200);
  
}

void loop() {
  /***
    Need to divide by 4 because analog inputs range from
    0 to 1023 and each byte of the EEPROM can only hold a
    value from 0 to 255.
  ***/


for (size_t i = 0; i < sizeof(potions) ; i++)
        {
        Serial.println(potions[i]);  //  "Сообщение: "
        delay(500);
        }
  int val = analogRead(0) / 4;

  /***
    Write the value to the appropriate byte of the EEPROM.
    these values will remain there when the board is
    turned off.
  ***/

  EEPROM.write(addr, val);

  /***
    Advance to the next address, when at the end restart at the beginning.

    Larger AVR processors have larger EEPROM sizes, E.g:
    - Arduno Duemilanove: 512b EEPROM storage.
    - Arduino Uno:        1kb EEPROM storage.
    - Arduino Mega:       4kb EEPROM storage.

    Rather than hard-coding the length, you should use the pre-provided length function.
    This will make your code portable to all AVR processors.
  ***/
  addr = addr + 1;
  if (addr == EEPROM.length()) {
    addr = 0;
  }

  /***
    As the EEPROM sizes are powers of two, wrapping (preventing overflow) of an
    EEPROM address is also doable by a bitwise and of the length - 1.

    ++addr &= EEPROM.length() - 1;
  ***/


  delay(100);
}
