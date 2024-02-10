
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#define PN532_IRQ (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield
#include "HX711.h"

#include <iarduino_IR_RX.h>  // Подключаем библиотеку для работы с ИК-приёмником
iarduino_IR_RX IR(8);        // Объявляем объект IR, с указанием вывода к которому подключён ИК-приёмник

// for sound
#include <SoftwareSerial.h>
#include "DYPlayerArduino.h"
SoftwareSerial mySerial(2, 3);  // RX, TX
DY::Player player(&mySerial);

HX711 scale;

uint8_t dataPin = 6;
uint8_t clockPin = 7;
float w1, w2, previous = 0;

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150   // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600   // This is the 'maximum' pulse length count (out of 4096)
#define USMIN 1000     // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX 1600     // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz updates
int eyelid_upper, eyelid_lower, eye_left, eye_right, eye_up, eye_down;
// our servo # counter
uint8_t servonum = 0;
String cardid = "0";
String cardid_prev = "0";

int mass = 0, mass_prev = 0, mass_prev_prev = 0, state = 0, state_prev = 0, glass_mass, expected_mass;
int cycle_counter = 0;
bool eye_is_up = false;
bool play_sound = true;

// each row contains glass RFID UID as an unsigned 32-bit int, glass mass measured in tenths of gram, expected mass to be weight in this glass measured in tenths of grams
String ingredient_list[4][3] = {
  { "4a142b15", "380", "100" },        // spine cup, to measure vinegar for peruvian night potion or water for stormglass
  { "3a1c7315", "335", "120" },        // twisted glass to measure alcohol for stormglass
  { "04598e1a237380", "495", "300" },  // goblet, to measure water for peruvian night potion
  { "3a0f8b15", "120", "36" }          // glass that looks like open flower to measure camphor for stormglass
};


void setup() {
  Serial.begin(115200);
  Serial.println("Servo test!");
  scale.begin(dataPin, clockPin);
  pwm.begin();

  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  Serial.println("NFC test!");
  delay(100);
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1)
      ;                         // halt
    scale.set_scale(42.00983);  // TODO you need to calibrate this yourself.
    scale.tare();

    Serial.print("UNITS: ");
    Serial.println(scale.get_units(10));
  }
  scale.begin(dataPin, clockPin);
  scale.set_scale(-193.00146);  // TODO you need to calibrate this yourself.
  scale.tare();

  for (uint16_t microsec = 1800; microsec > 1000; microsec--) {
    pwm.writeMicroseconds(4, microsec);
  }



  // eyelid_upper = 1695;
  // eyelid_lower = 1265;


  // pwm.writeMicroseconds(3, eyelid_upper);
  // pwm.writeMicroseconds(2, eyelid_lower);

  pwm.sleep();

  // sound setup
  player.begin();
  player.setVolume(25);

  IR.begin();  // Инициируем работу с ИК-приёмником
}

// You can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;

  pulselength = 1000000;      // 1,000,000 us per second
  pulselength /= SERVO_FREQ;  // Analog servos run at ~60 Hz updates
  Serial.print(pulselength);
  Serial.println(" us per period");
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength);
  Serial.println(" us per bit");
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

void loop() {
  cardid = readRFID();
  mass = scale.get_units(10);

  Serial.print("Current mass: ");
  Serial.println(mass);

  if (IR.check()) {           // Если в буфере имеются данные, принятые с пульта (была нажата кнопка)
    Serial.println(IR.data);  // Выводим код нажатой кнопки
    if ((IR.data == 1111000004) || (IR.data == 16716015)) {
      openeye(1000, 1550);
      state = 1;
    }
  }

  if ((cardid_prev != cardid) && (mass > 10) && (state > 0)) {
    Serial.print("  UID : ");
    Serial.println(cardid);
    Serial.println("");

    for (int i = 0; i < sizeof(ingredient_list) / sizeof(ingredient_list[0]); i++) {
      if (cardid == ingredient_list[i][0]) {
        glass_mass = ingredient_list[i][1].toInt();
        expected_mass = ingredient_list[i][2].toInt();

        Serial.print("Glass mass : ");
        Serial.println(glass_mass);
        Serial.println("");

        Serial.print("Expected contents mass: ");
        Serial.println(expected_mass);
        Serial.println("");

        state = 2;
      }
    }
    cardid_prev = cardid;
  }

  if (state == 2) {
    Serial.println("Glass with recognized RFID present");
    if (eye_up == 1550) {
      openeye(1550, 1800);
    } else {
      blink();
    }
    state = 3;
  }

  if (mass_prev != mass) { play_sound = true; }

  if (state == 3) {
    // if glass contains something and mass hasn't changed in the last 3 iterations
    if ((mass > glass_mass + 5) && (mass == mass_prev) && (mass_prev == mass_prev_prev)) {
      uint16_t contents_mass = mass - glass_mass;
      Serial.print("Contents mass: ");
      Serial.println(contents_mass);

      int contents_vs_expected = (contents_mass * 100) / expected_mass;

      if (contents_vs_expected < 60) {
        // less than 60% of the expected contents mass
        Serial.println("Too little!");
        if (play_sound) { blink(); }
        seteyelidposition(1265);
        if (play_sound) {
          player.playSpecified(4);  // says "more!"
          delay(3000);
        }
      } else if ((contents_vs_expected >= 60) && (contents_vs_expected < 90)) {
        // between 60% and 90% of the expected contents mass
        Serial.println("A little more!");
        if (play_sound) { blink(); }
        seteyelidposition(1300);
        if (play_sound) {
          player.playSpecified(2);  // says "a little bit more!"
          delay(3000);
        }
      } else if ((contents_vs_expected >= 90) && (contents_vs_expected <= 110)) {
        // between 90% and 110% of the expected contents mass
        Serial.println("Just right!");
        if (play_sound) { blink(); }
        seteyelidposition(1330);
        if (play_sound) {
          player.playSpecified(1);  // says "exactly!"
          delay(3000);
        }
        state = 1;
      } else if ((contents_vs_expected > 110) && (contents_vs_expected <= 160)) {
        // between 110% and 160% of the expected mass
        Serial.println("A little less!");
        if (play_sound) { blink(); }
        seteyelidposition(1365);
        if (play_sound) {
          player.playSpecified(5);  // says "a little less!"
          delay(3000);
        }
      } else {
        // more than 160% of the expected mass
        Serial.println("Too much!");
        if (play_sound) { blink(); }
        seteyelidposition(1400);
        if (play_sound) {
          player.playSpecified(3);  // says "less!"
          delay(3000);
        }
      }
      player.stop();
      play_sound = false;
    }
  }

  if ((cycle_counter > 300) && eye_is_up) {
    Serial.println("Inactive, going to sleep");
    lowereye();
    cycle_counter = 0;
    state = 0;
  }

  state_prev = state;
  mass_prev = mass;
  mass_prev_prev = mass_prev;
  if (eye_is_up) {
    cycle_counter++;
  }
}

String readRFID() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint32_t cardid = 0;

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);

  String hexString = "";
  for (int i = 0; i < uidLength; i++) {
    if (uid[i] < 0x10) {
      // If the value is less than 0x10, add a leading zero for better formatting
      hexString += "0";
    }
    hexString += String(uid[i], HEX);
  }

  return hexString;
}

void seteyelidposition(int eyelid_lower_new) {
  if (eyelid_lower_new > eyelid_lower) {
    pwm.wakeup();
    while (eyelid_lower < eyelid_lower_new) {
      eyelid_lower += 3;
      eyelid_upper -= 3;
      pwm.writeMicroseconds(2, eyelid_lower);
      pwm.writeMicroseconds(3, eyelid_upper);
    }
    eyelid_lower = eyelid_lower_new;
    pwm.writeMicroseconds(2, eyelid_lower);
    pwm.sleep();
  } else if (eyelid_lower_new < eyelid_lower) {
    pwm.wakeup();
    while (eyelid_lower > eyelid_lower_new) {
      eyelid_lower -= 3;
      eyelid_upper += 3;
      pwm.writeMicroseconds(2, eyelid_lower);
      pwm.writeMicroseconds(3, eyelid_upper);
    }
    eyelid_lower = eyelid_lower_new;
    pwm.writeMicroseconds(2, eyelid_lower);
    pwm.sleep();
  }
}

void openeye(int lift_from, int lift_to) {
  pwm.wakeup();

  //up
  for (uint16_t microsec = lift_from; microsec < lift_to; microsec++) {
    pwm.writeMicroseconds(4, microsec);
  }
  eye_up = lift_to;

  eyelid_upper = 1600;
  eyelid_lower = 1265;
  //open
  for (uint16_t microsec = 0; microsec < 40; microsec++) {
    eyelid_upper = eyelid_upper - 3;
    eyelid_lower = eyelid_lower + 3;
    pwm.writeMicroseconds(3, eyelid_upper);
    pwm.writeMicroseconds(2, eyelid_lower);
  }
  delay(555);

  for (uint16_t microsec = 0; microsec < 35; microsec++) {
    eyelid_upper = eyelid_upper - 3;
    eyelid_lower = eyelid_lower + 3;
    pwm.writeMicroseconds(3, eyelid_upper);
    pwm.writeMicroseconds(2, eyelid_lower);
  }

  delay(222);
  for (uint16_t microsec = 1000; microsec < 1200; microsec++) {
    pwm.writeMicroseconds(1, microsec);
  }

  delay(222);
  for (uint16_t microsec = 1200; microsec > 1000; microsec--) {
    pwm.writeMicroseconds(1, microsec);
  }

  for (uint16_t microsec = 1000; microsec < 1050; microsec++) {
    pwm.writeMicroseconds(1, microsec);
  }

  if (lift_to == 1800) {
    eye_is_up = true;
  }

  delay(100);

  for (uint16_t microsec = 50; microsec > 1; microsec--) {
    eyelid_upper = eyelid_upper + 3;
    eyelid_lower = eyelid_lower - 3;
    pwm.writeMicroseconds(3, eyelid_upper);
    pwm.writeMicroseconds(2, eyelid_lower);
  }

  delay(555);
  for (uint16_t microsec = 850; microsec < 1450; microsec++) {
    microsec++;  //microsec++;
    pwm.writeMicroseconds(0, microsec);
  }
  delay(555);
  for (uint16_t microsec = 1450; microsec > 850; microsec--) {
    microsec--;  // microsec--;
    pwm.writeMicroseconds(0, microsec);
  }
  for (uint16_t microsec = 850; microsec < 1200; microsec++) {
    microsec++;  // microsec++;
    pwm.writeMicroseconds(0, microsec);
  }

  //close

  // for (uint16_t microsec = 5; microsec > 1; microsec--) {
  //   eyelid_upper = eyelid_upper + 3;
  //   eyelid_lower = eyelid_lower - 3;
  //   pwm.writeMicroseconds(3, eyelid_upper);
  //   pwm.writeMicroseconds(2, eyelid_lower);
  // }

  pwm.sleep();
}

void blink() {
  int eyelid_lower_old = eyelid_lower;
  seteyelidposition(1230);
  seteyelidposition(eyelid_lower_old);
}

void lowereye() {
  //down
  seteyelidposition(1265);
  pwm.wakeup();
  for (uint16_t microsec = eye_up; microsec > 1000; microsec--) {
    pwm.writeMicroseconds(4, microsec);
  }
  pwm.sleep();
  eye_is_up = false;
}