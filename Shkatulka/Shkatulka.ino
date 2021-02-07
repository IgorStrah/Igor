
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9


MFRC522 mfrc522(SS_PIN, RST_PIN);
String uidDec;  // для храниения номера метки в десятичном формате

#include <IRremote.h>
int RECV_PIN = 7;
int openclose=0;
int stepread = 0;
unsigned long irvalue;
IRrecv irrecv(RECV_PIN);

decode_results results;



const char  *potions[100] =
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
  "43F9DF2A96A80",//   Волосы ламии
  "4629EF2A96A81",//   Сопли тролля
  "4029EF2A96A81",//   Шкура экроцирпуса
  "4FF9FF2A96A80",//   Слизь Флоббер-Червя
  "4079FF2A96A81",//   Слизб болотника
  "4B39DF2A96A80",//   яд василиска
  "40B9FF2A96A81",//   кровь саламандры
  "44F9DF2A96A80",//   Плоть висильника
  "41D9EF2A96A80",//   Сок пиявки
  "4BF9FF2A96A80",//   Порошок рога единорога
  "4F69EF2A96A80",//   Лунны осминог
  "4119EF2A96A80",//   Сок ягоды БУМ!
  "4479DF2A96A80",//   Яйца докси
  "44B9DF2A96A80",//   травной стручёк
  "48E9EF2A96A81",//   Змеиные Клыки
  "44E9EF2A96A81",//   Капли мёртвой воды
  "4F29EF2A96A80",//   Спиртовая настойка
  "4629EF2A96A80",//   Паучье яйцо
  "4569EF2A96A81",//   Иглы дикообраза
  "4A49EF2A96A80",//   Пичвочны сок
  "4729EF2A96A80",//   Кровь великана   !!!
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
  "4469EF2A96A81",//   Сушёная крапива
  "4319DF2A96A81",//   Порошок из костей фестрала
  "4DE9EF2A96A80",//   Жилы кентавра
  "46A9EF2A96A80",//     Лоскут одежды Баньши
  "43743921F6F81",//     Лапка детёныша каппы
  "45E9EF2A96A80",//     Жилы дракона
  "4429EF2A96A81",//     Мята
  "44D9DF2A96A81",//     Рогатые слизни
  "4159EF2A96A80",//     Морские Конькт
  "42D9DF2A96A81",//     Капли лунно росы
  "44A9EF2A96A81",//     Кора волшебно рябины
  "4434412AC6A81",//     Сушоные жала веретеницы
  "4529EF2A96A80",//     Крылья феи
  "XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв
"XXXXXXXXXXXXX",//     Резерв







 
};


#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

// implement a notification class,
// its member methods will get called 
//
int n;
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};

// instance a DFMiniMp3 object, 
// defined with the above notification class and the hardware serial class
//
//DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial1);

// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definition and uncomment these lines
SoftwareSerial secondarySerial(A2,A3); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);


byte readis;

void setup() {
  Serial.begin(115200);

  SPI.begin();  //  инициализация SPI / Init SPI bus.
  mfrc522.PCD_Init();     // инициализация MFRC522 / Init MFRC522 card.

  delay (100);
  irrecv.enableIRIn(); // Start the receiver
  mp3.begin();
  mp3.setVolume(22);
  
}
void loop() {

if (openclose==0)
{
 
  if (irrecv.decode(&results)) {


    irvalue = results.value;
    irrecv.resume(); // Receive the next value
    Serial.println(irvalue);

    if ((irvalue == 1111000004) || (irvalue == 16726215)) { // атака проведена верно  - сворачиваемся.

      irvalue = "";
      Serial.println("play");
      openclose=1;
      mp3.playFolderTrack(2, 1); // sd:/01/001.mp3
    }
  }
}

if (openclose==1){

if (irrecv.decode(&results)) {


    irvalue = results.value;
    irrecv.resume(); // Receive the next value
    

    }

    // ищем новые карты:
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }

    // выбираем одну из карт:
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }
    // показываем UID на мониторе порта:

    String content = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    content.toUpperCase();
    uidDec = content.substring(1);
     Serial.println("UID : ");  //  "Сообщение: "
    Serial.println(content.substring(1));  //  "Сообщение: "

delay(100);


    for (size_t i = 0; i < 100 ; i++)
    {

      if (uidDec==potions[i])
      {
        
      mp3.playFolderTrack(1, i+1); // sd:/01/001.mp3
      Serial.print(potions[i]);  //  "Сообщение: "
      Serial.print("  i ");  //  "Сообщение: "
      Serial.println(i);  //  "Сообщение: "
      delay(1000);
      openclose=0;
      }
      
   }
}
}
