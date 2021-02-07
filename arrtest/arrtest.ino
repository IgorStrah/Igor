


//potions
int clocpationsl;// счётчик совпадени. как только он равен размеру массива, значит это зелье сварено
int potioncloc;

const char *potions[][10]  =
{
  {
    "4439DF2A96A80",   // 0
    "4219EF2A96A80",     // 1
    "40E9DF2A96A80",     // 2
    "40D9EF2A96A80",     // 3
    "4749FF2A96A81",     // 4
    "4099EF2A96A80",     // 5
    "x",     // 6
    "x",     // 7
    "x",     // 8
    "x",     // 9
    
  },

  {
    "4629EF2A96A81",   // 0
    "40A9DF2A96A80",     // 1
    "43D9DF2A96A81",     // 2
    "4749FF2A96A81",     // 3
    "4849FF2A96A81",     // 4
    "44B9DF2A96A80",     // 5
       "x",     // 6
    "x",     // 7
    "x",     // 8
    "x",     // 9
  }

};



void setup() {
  Serial.begin(115200);
  Serial.println("Waiting for card...");

}
void loop() {



  Serial.print("data_set" );
  Serial.println(sizeof(potions) / sizeof(potions[0]));


  for (size_t i = 0; i < sizeof(potions) / sizeof(potions[0]); i++)
  {
    Serial.print("   i   " );
    Serial.println(i);
    Serial.print("potions" );
    Serial.println(sizeof(potions[i]));
    delay(50);

    Serial.print("   sizeof(potions[i])/ sizeof(potions[i][0])   " );
    Serial.println(sizeof(potions[i]) / sizeof(potions[i][0]));

    for (size_t j = 0; j < sizeof(potions[i]) / sizeof(potions[i][0]); j++)
    {
      if ( potions[i][j] != "")
      {
           

        Serial.println( potions[i][j]);
        delay(80);
 
      }

  }



  }}
