
// !!Uwaga!! wymaga instalacji dodatkowej biblioteki "servo"
// 1. w Menu u góry wybrać Tools->Manage Libraries (Ctrl+Shift+I) 
// 2. w polu search wpisać "servo"
// 3. wybrać na liście Servo by arduino v1.2.2 (obecna wersja)
// 4. zainstalować i odpalić
// 5. dobrać parametry 
/////////////////////////////////////////////////////////////////////////
// Wybrać płytkę Tools->Board, "Arduino nano"
// Wybrac procesor "ATMega328P (Old Bootloader), lub inny w zależności od tego co zadziała
//

///////////////////////////////////////////////// używanie /////////////////////////////////////
// po wlaczeniu zasilania autoatycznie idzie na pozycje min i max potem wraca na środek zakresu
// potem automatycznie losuje odstep czasu z zakresu RAND_TIME_MIN_SEK .. RAND_TIME_MAX_SEK
// za każdym razem losuje liczbę ruchow w zakresie RAND_MIN_SZT .. RAND_MAX_SZT
// dla każdego z ruchu losuje pozycję w zakresu RAND_MIN_ANGLE .. RAND_MAX_ANGLE
// oraz predkosc ruchu, a raczej opóźnienie ruchu (im wyższa wartość tym wolniejszy ruch) RAND_MAX_DELAY_SPEED_MS .. RAND_MIN_DELAY_SPEED_MS
// bonus:
// reczne sterowanie via serial port:
// wpisanie liczby x\n (ENTER) powoduje ruch serwo na kąt x, przykład: 45<ENTER> ustawi ramie na kat 45 stopni
// wpisanie litery 'r'\n (ENTER) powoduje powrót do trybu automatycznego
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>

/////////////////// konfiguracja parametrów //////////////////////////////////////////////////

#define RAND_TIME_MIN_SEK 60 //ruszaj co min 60s 
#define RAND_TIME_MAX_SEK 4*60 //max 4 min

#define RAND_MIN_ANGLE 80 //zakres ruchu
#define RAND_MAX_ANGLE 120

#define RAND_MIN_DELAY_SPEED_MS 1
#define RAND_MAX_DELAY_SPEED_MS 20

#define RAND_MIN_SZT 1
#define RAND_MAX_SZT 20

int servoPin = 7;

/////////////////////////////// koniec konfiguracji poniżej nic nie zmieniać //////////////////

Servo servo;  

int angle = RAND_MIN_ANGLE+(RAND_MAX_ANGLE-RAND_MIN_ANGLE)/2;   // servo position in degrees 

void setup()
{
delay(5000);
Serial.begin(115200);
servo.attach(servoPin);
Serial.println("Setup servo start");
randomSeed(analogRead(0));
Serial.print("- Servo min angle: ");
servo.write(RAND_MIN_ANGLE);
Serial.println(RAND_MIN_ANGLE);
delay(5000);
Serial.print("- Servo max angle: ");
servo.write(RAND_MAX_ANGLE);
Serial.println(RAND_MAX_ANGLE);
delay(5000);
servo.write(angle);
Serial.println("Setup servo end.");
} 

unsigned long ms=0;
long randTime=RAND_TIME_MIN_SEK;
long randSpeedDelay=RAND_MIN_DELAY_SPEED_MS;
long randAngle= RAND_MIN_ANGLE;
long randSzt=RAND_MIN_SZT;

bool startButton=false;
bool popStartButton=false;

long lastCzekamy=0;
String inString = "";  // string to hold input
bool manual=false;
void loop()
{
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (isDigit((int)inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    if(inChar=='r')
    {
      manual=false;
      Serial.println("resume.");
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      Serial.print("Angle:");
      Serial.println(inString.toInt());
      Serial.print("Received: ");
      Serial.println(inString);
      // clear the string for new input:
     
      if(inString.toInt()>0)
      {
       angle=inString.toInt();
       servo.write(angle);
       manual=true;
       Serial.print("manual.");
      }
      inString = "";
      
    }
  }

  if(!manual)
  {

  if(false && popStartButton==false)
  {
    startButton=true;
    popStartButton=true;
  }else
  {
     popStartButton=false;
  }


  if(millis()-ms>randTime*1000 || startButton==true)
  {
    startButton=false;
    randTime  =  random(RAND_TIME_MIN_SEK,RAND_TIME_MAX_SEK);
    
    randSzt=random(RAND_MIN_SZT,RAND_MAX_SZT);
    Serial.print("Ruch start, sztuk: ");Serial.println(randSzt);

    for(int szt=0;szt<randSzt;szt++)
    {
      Serial.print("- sztuka: ");Serial.print(szt);Serial.print(" z ");Serial.println(randSzt);
      randSpeedDelay = random(RAND_MIN_DELAY_SPEED_MS,RAND_MAX_DELAY_SPEED_MS);
      randAngle= random(RAND_MIN_ANGLE,RAND_MAX_ANGLE);
      Serial.print("-- Pozycja: ");Serial.print(angle);Serial.print(", nowa: ");Serial.print(randAngle);Serial.print("; delay: ");Serial.println(randSpeedDelay);
      Serial.print("--- Pozycja: ");
      if(randAngle>angle)
      {
  
          // move arm
          
          for(int a = angle; a < randAngle; a++)
          {
            angle=a;
            servo.write(angle);
          Serial.print(angle);Serial.print(", ");
            delay(randSpeedDelay);
          }
      }else {
        for(int a = angle; a > randAngle; a--)
        {
              angle=a;
              servo.write(angle);
              Serial.print(angle);Serial.print(", ");
              delay(randSpeedDelay);
         }
      }//else
      Serial.println(" koniec sztuki.");
      }// for szt
      ms=millis();
      Serial.print("Nastepny ruch za: ");Serial.println(randTime);
      Serial.print(".. czekamy jeszcze: ");
    }// if millis
    
    long x=millis()-ms-randTime*1000;
    if(x % 1000==0 && x!=lastCzekamy)
    {
       lastCzekamy=x;
       Serial.print(-x/1000);Serial.print(", ");
       if(x/1000%60==0)Serial.println("");
    }
  } }
