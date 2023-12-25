
String ircod;
String updws, lrs;
int  updw, lr, updwold, lrold, updwmap, lrmap, x, y, xr, xl, yu, yd; // ох. не завидую я тебе когда ты будешь пытаться это понять.
int  fupdw, flr;
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        A0 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 1 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);



#include <IRremote.h>
#include <AccelStepper.h>
  
int RECV_PIN = 10;
int red, gren, blue;
int average1,average2;
uint8_t startgame, cloc, num;
int motorStep, motorStep2;
const int stepsPerRevolution = 200;
AccelStepper stepper1(AccelStepper::FULL4WIRE, 5, 3, 4, 2);
AccelStepper stepper2(AccelStepper::FULL4WIRE, 6, 8, 7, 9);



#define NUM_AVER 5       // выборка (из скольки усредняем)
long average;             // перем. среднего
int valArray[NUM_AVER][2];   // массив

byte idx = 0;             // индекс

int stepCount = 0;  // number of steps the motor has taken

    int motorSpeed=0;
    int motorSpeed2=0 ;
    int motorSpeedL=0;
    int motorSpeedR=0 ;
    int motorSpeedU=0;
    int motorSpeedD=0 ;
IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(115200);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
  
  startgame = 0;

    stepper1.setMaxSpeed(200.0);
    stepper1.setAcceleration(200.0);
    
    stepper2.setMaxSpeed(200.0);
    stepper2.setAcceleration(200.0);
 

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  
}

void loop() {

 
if (irrecv.decode(&results)) {
  
 
    irrecv.resume(); // Receive the next value
    ircod = results.value;

    // Serial.println(ircod); 

if ((results.value == 1111000001)||(results.value == 16726215)) // атака проведена верно  - сворачиваемся.
{
  startgame=1;
Serial.print(startgame);
     Serial.print(" startgame ");
  
}

    
 if ((results.value == 1111000004)||(results.value == 16726215)) // атака проведена верно  - сворачиваемся.
{
  Serial.println("Enabled go back");
  
  startgame=0;
  stepper1.setMaxSpeed(350);
  stepper2.setMaxSpeed(350);
  stepper1.moveTo(0);
  stepper2.moveTo(0);
  red=0; gren=0; blue=0;
while ((stepper1.currentPosition()!=0)&&(stepper2.currentPosition()!=0))
{
    stepper1.run();
    stepper2.run();
}

    stepper1.stop(); 
    stepper2.stop(); 
    stepper1.disableOutputs(); 
    stepper2.disableOutputs(); 
}


 
    lr=lrmap;updw=updwmap; 
    // Serial.println(results.value);

    updws = ircod.substring(0, 2);
    lrs = ircod.substring(3, 5);
    updw = updws.toInt();
    lr = lrs.toInt();

middleArifm(updw,lr);
updw=average1;
lr=average2;


     Serial.print(updw);
     Serial.print(" ");
     Serial.print(lr);
     Serial.println("    :     ");

 
  if (startgame == 1) // start game and calibrate acelerometr.Its a first start.
  {
    if ((updwold == updw) && (lrold == lr)&& (lr != 0)&&(updw != 0))
    {
      num++;

     red+=20, gren+=20, blue+=20;
     pixels.setPixelColor(0, pixels.Color(red, gren, blue));
     pixels.show(); // Set all pixel colors to 'off'

    }
    else
    {
      num = 0;
       pixels.clear(); // Set all pixel colors to 'off'
    }
    if (num > 3)
    {
    updwmap = updw;  lrmap = lr;
    startgame = 2;
    Serial.print(" Start! ");
    
  pixels.setPixelColor(0, pixels.Color(100, 100, 100));
  pixels.show();   // Send the updated pixel colors to the hardware.
 
    }
 
  pixels.setPixelColor(0, pixels.Color(motorSpeed2, motorSpeedD, motorSpeed));
  pixels.show();   // Send the updated pixel colors to the hardware.
    updwold = updw;
    lrold = lr;
   // delay(5);
  }


  if (startgame == 2)
  {

    x = updw - updwmap;
    y = lr - lrmap;
    if (x > 1)
    {
      xr = x;
      xl = 0;
    } 
    else if (x < -1)
    {
      xl = x * (-1);
      xr = 0;
    }

    else if (x == 0)
    {
      xl = 0;
      xr = 0;
    }

    if (y > 1)
    {
      yd = y;
      yu = 0;
    }
    else if (y < -1)
    {
      yu = y * (-1);
      yd = 0;
    }

       else if (y==0)
    {
      yu = 0;
      yd = 0;
    }

   // логика какая вверх низ палочкой  это движение строго вверх или в низ сразу оба двигателя.
   // при этом при повороте палочки, право/лево. правый/левый двигатель тормозят. 

    if (xr>3) 
    {
    motorSpeedL = map(xr, 0, 25, 0, 200);
    motorSpeedR =0;
    }
    else if  (xl>3) 
    {
     motorSpeedR = map(xl, 0, 25, 0, 200);
     motorSpeedL =0;
    }
    
    if (yd>3) 
    { 
    motorSpeedD = map(yd, 0, 25, 0, 300);
    motorSpeedU = 0;
    }
    else if(yu>3) 
    {
    motorSpeedU = map(yu, 0, 25, 0, 300);
    motorSpeedD = 0;
    }


    if ( (xr==0) &&(xl==0) ) 
    
    {
    motorSpeedL = 0;
    motorSpeedR =0;
    }
    
    if ((yu==0)&&(yd==0) &&(xr==0) &&(xl==0) ) 
    {
    motorSpeedU = 0;
    motorSpeedD =0;
    motorSpeedL = 0;
    motorSpeedR =0;
    }

    if (motorSpeedU>5)
    {
     motorSpeed=motorSpeedU-( map(motorSpeedL, 0, 100, 0, motorSpeedU));
     motorSpeed2=motorSpeedU-( map(motorSpeedR, 0, 100, 0, motorSpeedU));
    }
    else if (motorSpeedD>5)
    {
     motorSpeed= (motorSpeedD-( map(motorSpeedL, 0, 100, 0, motorSpeedD)))*(-1);
     motorSpeed2=(motorSpeedD-( map(motorSpeedR, 0, 100, 0, motorSpeedD)))*(-1);
    }
     
   if (((motorSpeedU==0)&&(motorSpeedD==0)) || ((abs(motorSpeed)>350)&&(abs(motorSpeed2)>350)) )
    { 
    stepper1.stop(); 
    stepper2.stop(); 
    stepper1.disableOutputs(); 
    stepper2.disableOutputs(); 
    } 
    else
    {
  pixels.setPixelColor(0, pixels.Color(motorSpeed2, motorSpeedD, motorSpeed));
  pixels.show();   // Send the updated pixel colors to the hardware.
 
     stepper1.setMaxSpeed(abs(motorSpeed));
     stepper2.setMaxSpeed(abs(motorSpeed2));
     stepper1.move(motorSpeed);
     stepper2.move(motorSpeed2);
  
  
  //   Serial.print(" mystepper.currentPosition(); ");
  //   Serial.print(stepper1.currentPosition());
  //   Serial.print(" mystepper.currentPosition(); ");
  //   Serial.println(stepper2.currentPosition());

     
     Serial.print(xr);
     Serial.print(" ");
     Serial.print(xl);
     Serial.print(" ");
     Serial.print(yd);
     Serial.print(" ");
     Serial.println(yu);

    
    }

/* 
     Serial.print(" xr ");
     Serial.print(xr);
     Serial.print(" xl ");
     Serial.print(xl);
     Serial.print(" yd ");
     Serial.print(yd);
     Serial.print(" yu ");
     Serial.print(yu);
  */   

 /* 
     Serial.print(" motorSpeedU ");
     Serial.print(motorSpeedU);
     Serial.print(" motorSpeedD ");
     Serial.print(motorSpeedD);
     Serial.print(" motorSpeedL ");
     Serial.print(motorSpeedL);
     Serial.print(" motorSpeedR ");
     Serial.print(motorSpeedR);
     
     */
/*
      
    Serial.print(" motorSpeed ");
    Serial.print(motorSpeed);
    Serial.print(" motorSpeed2 ");
    Serial.println(motorSpeed2);
*/
   
    
  } 
 
}
      stepper1.run();
      stepper2.run();
}


void irrecvdata(void)
{
    if (irrecv.decode(&results)) {
    cli();
    lr=lrmap;updw=updwmap; 
    // Serial.println(results.value);
    irrecv.resume(); // Receive the next value
    ircod = results.value;
    updws = ircod.substring(0, 2);
    lrs = ircod.substring(3, 5);
    updw = updws.toInt();
    lr = lrs.toInt();
    sei();
 
  }
}


int middleArifm(int newVal1,int newVal2) {       // принимает новое значение
  valArray[NUM_AVER-1][0] = newVal1;            // пишем каждый раз в новую ячейку
  valArray[NUM_AVER-1][1] = newVal2;           // пишем каждый раз в новую ячейку
  
  average1 = 0;
  average2 = 0;// обнуляем среднее
  
  for (int i = 0; i < NUM_AVER; i++) {
    average1 += valArray[i][0];         // суммируем
    average2 += valArray[i][1];  
   
    if (i<=NUM_AVER-2)
    {
    valArray[i][0]= valArray[i+1][0];         // суммируем
    valArray[i][1]= valArray[i+1][1];         // суммируем
    }
  }
  
  average1 /= NUM_AVER;              // делим
  average2 /= NUM_AVER;              // делим
                   // возвращаем
}
