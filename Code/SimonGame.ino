////////////////////////////////////////////////////////////////////////////////////////
// OVERVIEW
////////////////////////////////////////////////////////////////////////////////////////
/*
Date: 17:26 30th July, 2024.

DESCRIPTION: Classic Simon Game.

AUTHOR: IO Exception.

COMMENTS: 
The Classic Simon Game implemented on Arduino.

Three Games included!
  1. Simon Says (Set different Levels)
  2. Player Says
  3. Add the Color (MultiPlayer).

HISTORY:

*/

//////////////////////////////////////////////////////////////////////////
//// INCLUDE /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
//// DEFINE //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define redLED        4
#define greenLED      5
#define blueLED       6
#define yellowLED     7

#define toneButton    3
#define redButton     8
#define greenButton   9
#define blueButton   10
#define yellowButton 11
#define lastButton   12

#define NOTE_E4      329                  // RED
#define NOTE_G4      392                 // GREEN
#define NOTE_G3      196                // BLUE
#define NOTE_C4      261               // YELLOW
#define NOTE_C5      523             
#define duration     250

#define BCD_A        A0              // LSB   
#define BCD_B        A1      
#define BCD_C        A2
#define BCD_D        A3              
#define BCD_A1       A4              
#define BCD_B1       A5            // MSB

//////////////////////////////////////////////////////////////////////////
//// GLOBAL //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
uint8_t gSeqCount = 1;
uint8_t gMultiCount = 2;
uint8_t turns = 0;
uint8_t count = 0;
bool gFlag = true;
uint8_t gDifficulty = 8;
uint8_t gMode = 1;
uint8_t gMultiIndex = 0;

//////////////////////////////////////////////////////////////////////////
//// ARRAYS //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int LEDArray[] = {redLED, greenLED, blueLED, yellowLED, redLED, greenLED, blueLED, yellowLED, greenLED, yellowLED, greenLED};
int notesArray[] = {NOTE_E4, NOTE_G4, NOTE_G3, NOTE_C4, NOTE_C4, NOTE_G3, NOTE_G4, NOTE_E4, 1047, 523 ,1047};

char colors[4][7] = {"RED", "GREEN", "BLUE", "YELLOW"};
int BCDPins[] = {BCD_A, BCD_B, BCD_C, BCD_D, BCD_A1, BCD_B1};
int LEDPins[] = {redLED, greenLED, blueLED, yellowLED};

char randomColorSequence[31][7];
char userInputSequence[31][7];
char playerSaysSequence[31][7];
char multiPlayerSequence[31][7];

//////////////////////////////////////////////////////////////////////////
//// SETUP ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void setup()
{
// Initialise Buttons
pinMode(redButton, INPUT_PULLUP);
pinMode(greenButton, INPUT_PULLUP);
pinMode(blueButton, INPUT_PULLUP);
pinMode(yellowButton, INPUT_PULLUP);
pinMode(lastButton, INPUT_PULLUP);

// Initialise LEDs
pinMode(redLED, OUTPUT);
pinMode(greenLED, OUTPUT);
pinMode(blueLED, OUTPUT);
pinMode(yellowLED, OUTPUT);
pinMode(toneButton, OUTPUT);

// Initialize Analog Pins
pinMode(BCD_A, OUTPUT);
pinMode(BCD_B, OUTPUT);
pinMode(BCD_C, OUTPUT);
pinMode(BCD_D, OUTPUT);
pinMode(BCD_A1, OUTPUT);
pinMode(BCD_B1, OUTPUT);

Serial.begin(115200);

// prompt the user to set the Game Mode.
gameMode();

#if 1
switch( gMode )
  {
  case 1:
    selectDifficulty();
    startingSequence();
    start();
    delay(1000);
    initializeRandomColorSequence();
    break;
  case 2:
    selectDifficulty();
    startingSequence();
    start();
    delay(1000);
    playerSays();
    break;
  case 3:
    gDifficulty = 31;
    startingSequence();
    start();
    delay(1000);
    multiPlayerFirstTurn();
    break;
  }// end switch.
#endif

}// end setup.

//////////////////////////////////////////////////////////////////////////
//// LOOP ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void loop() 
{
#if 1

switch( gMode )
{
  case 1:
    simonSays(randomColorSequence);
    break;
  case 2:
    simonSays(playerSaysSequence);
    break;
  case 3:
    Serial.println("Entered case 3");
    multiPlayer(multiPlayerSequence);
    break;
}// end switch.

// Serial.println(" Exiting loop");
#endif

}// end loop.

////////////////////////////////////////////////////////////////////////////
//// SIMONSAYS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void simonSays(char genericArray[31][7])
{
//Serial.println("Executing Simon Says");
turns = 0;
count = 0;
bool sequenceComplete = true;

// Start the game set gFlag to true.
if( (gFlag) && (gSeqCount < gDifficulty)) 
  {
  showSequence(genericArray);

  while ( turns < gSeqCount )
        {
        //Serial.println("Cont.");
        int buttonState = 0;

        if( digitalRead( lastButton ) == 0) 
          {
          sequenceComplete = false;
          break;
          }

        if( digitalRead( redButton) == 0 )         buttonState = 1;
        else if( digitalRead( greenButton ) == 0)  buttonState = 2;
        else if( digitalRead( blueButton ) == 0)   buttonState = 3;
        else if( digitalRead( yellowButton ) == 0) buttonState = 4;
        
        switch( buttonState )
          {
          case 1:
            // Copy the input which user has giving to the userInputArray.
            strcpy(userInputSequence[count], "RED");
            // Light up the LED so user know he/she has press that buttom.
            digitalWrite(redLED, HIGH);
            delay(100);
            digitalWrite(redLED,LOW);
            // Generate tone.
            tone(toneButton, NOTE_E4 , duration);
            checkSequence (genericArray);
            break;
          case 2:
            strcpy(userInputSequence[count], "GREEN");
            digitalWrite(greenLED, HIGH);
            delay(100);
            digitalWrite(greenLED,LOW);
            tone(toneButton, NOTE_G4 , duration);
            checkSequence(genericArray);
            break;
          case 3:
            strcpy(userInputSequence[count], "BLUE");
            digitalWrite(blueLED, HIGH);
            delay(100);
            digitalWrite(blueLED,LOW);
            tone(toneButton, NOTE_G3, duration);
            Serial.println("Checking...");
            checkSequence(genericArray);
            break;
          case 4:
            strcpy(userInputSequence[count], "YELLOW");
            digitalWrite(yellowLED, HIGH);
            delay(100);
            digitalWrite(yellowLED,LOW);
            tone(toneButton, NOTE_C4 , duration);
            checkSequence(genericArray);
            break;
          }// end switch.
        }// end while.
//Serial.println("Exiting While\n");
  
  // Increment the Global Sequence Counter. Very Important!
  if( sequenceComplete )
    {
    gSeqCount++;

    setScore(count);

    }
  // Serial.print("Exiting simonSays\n");
  }// end if (Flag)
else restart();
}// end simonSays.

////////////////////////////////////////////////////////////////////////////
//// PLAYERSAYS ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void playerSays()
{
uint8_t index = 0;
bool flag = true;
// Serial.println("Entering Players Says");

while( flag )
  {
  int buttonState = 0;
  setScore(index);

  if( digitalRead(lastButton) == 0)
    {
    flag = false;
    break;
    }

  if( digitalRead( redButton) == 0 )         buttonState = 1;
  else if( digitalRead( greenButton ) == 0)  buttonState = 2;
  else if( digitalRead( blueButton ) == 0)   buttonState = 3;
  else if( digitalRead( yellowButton ) == 0) buttonState = 4;

  switch( buttonState )
    {
    case 1:
      strcpy(playerSaysSequence[index], "RED");
      digitalWrite(redLED, HIGH);
      delay(100);
      digitalWrite(redLED,LOW);
      delay(200);
      tone(toneButton, NOTE_E4 , duration);
      index++;
      break;
    case 2:
      strcpy(playerSaysSequence[index], "GREEN");
      digitalWrite(greenLED, HIGH);
      delay(100);
      digitalWrite(greenLED,LOW);
      delay(200);
      tone(toneButton, NOTE_G4 , duration);
      index++;
      break;
    case 3:
      strcpy(playerSaysSequence[index], "BLUE");
      digitalWrite(blueLED, HIGH);
      delay(100);
      digitalWrite(blueLED,LOW);
      tone(toneButton, NOTE_G3 , duration);
      delay(200);
      index++;
      break;
    case 4:
      strcpy(playerSaysSequence[index], "YELLOW");
      digitalWrite(yellowLED, HIGH);
      delay(100);
      digitalWrite(yellowLED,LOW);
      delay(200);
      tone(toneButton, NOTE_C4 , duration);
      index++;
      break;
    }// end switch.
  }// end while

}// end playerSays.

////////////////////////////////////////////////////////////////////////////
//// MULTIPLAYER //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void multiPlayer(char genericArray[31][7])
{
turns = 0;
count = 0;
// Serial.println("Entered MultiPlayer");
  if( (gFlag) && (gMultiCount < gDifficulty) )
  {
    while( turns < gMultiCount )
      {
      int buttonState = 0;

      if( digitalRead( redButton) == 0 )         buttonState = 1;
      else if( digitalRead( greenButton ) == 0)  buttonState = 2;
      else if( digitalRead( blueButton ) == 0)   buttonState = 3;
      else if( digitalRead( yellowButton ) == 0) buttonState = 4;
      
      switch(buttonState)
        {
        case 1:
          // Copy the input which user has giving to the userInputArray.
          strcpy(userInputSequence[count], "RED");
          // Light up the LED so user know he/she has press that buttom.
          digitalWrite(redLED, HIGH);
          delay(100);
          digitalWrite(redLED,LOW);
          // Generate tone.
          tone(toneButton, NOTE_E4 , duration);
          if( (gMultiCount - turns) == 1){ strcpy(multiPlayerSequence[turns], "RED"); turns++; delay(200); break;}
          else {checkSequence (genericArray); break;}
        case 2:
          strcpy(userInputSequence[count], "GREEN");
          digitalWrite(greenLED, HIGH);
          delay(100);
          digitalWrite(greenLED,LOW);
          tone(toneButton, NOTE_G4 , duration);
          if( (gMultiCount - turns) == 1){ strcpy(multiPlayerSequence[turns], "GREEN"); turns++; delay(200); break; } 
          else {checkSequence (genericArray); break;}
        case 3:
          strcpy(userInputSequence[count], "BLUE");
          digitalWrite(blueLED, HIGH);
          delay(100);
          digitalWrite(blueLED, LOW);
          tone(toneButton, NOTE_G3 , duration);
          if( (gMultiCount - turns) == 1){ strcpy(multiPlayerSequence[turns], "BLUE"); turns++; delay(200); break; }
          else { checkSequence (genericArray); break;}
        case 4:
          strcpy(userInputSequence[count], "YELLOW");
          digitalWrite(yellowLED, HIGH);
          delay(100);
          digitalWrite(yellowLED, LOW);
          tone(toneButton, NOTE_C4 , duration);
          if( (gMultiCount - turns) == 1){ strcpy(multiPlayerSequence[turns], "YELLOW"); turns++; delay(200); break; }
          else { checkSequence (genericArray); break;}
        }// end switch.
      }// end while.
    // Serial.println("exiting while");

  gMultiCount++;
  setScore(count);
  }// end if (Flag).
else restart();
}// end chooseYourColor.

////////////////////////////////////////////////////////////////////////////
//// INITIALIZERANDOMCOLORSEQUENCE ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void initializeRandomColorSequence()
{
randomSeed(millis());
for( int index = 0; index < 32 ; index++)
  {
  // Generate random numbers between 0 to 3;
  int secret = random(0,4);
  
  // Copy the randomly generated color to the randomColorSequence array. 
  strcpy(randomColorSequence[index], colors[secret]);
  }// end for
}

void multiPlayerFirstTurn()
{
delay(500);
randomSeed(millis());
int secret = random(0,4);
digitalWrite(LEDPins[secret], HIGH);
delay(200);
digitalWrite(LEDPins[secret], LOW);
delay(200);
tone(toneButton, notesArray[secret] , duration);
strcpy( multiPlayerSequence[0], colors[secret]);
}// end multiPlyerFirstTurn

////////////////////////////////////////////////////////////////////////////
//// GAMEMODE /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void gameMode()
{
bool flag = true;
uint8_t mode = 16;
setScore(mode);

while( (flag) )
  {
  //Serial.println("Entered While");

  if( digitalRead(yellowButton) == 0)
    {
    mode += 16;
    if( mode > 48)
      {
      mode = 16;
      }// end inner if
    setScore(mode);
    tone(toneButton, NOTE_C5 , duration);
    delay(200);
    }
    else if( digitalRead(greenButton) == 0){ flag = false; delay(200); }
  else
    {
    digitalWrite(yellowLED, HIGH);
    delay(100);
    digitalWrite(yellowLED, LOW);
    delay(100);
    }
  }// end while

setScore(0);

switch( mode )
  {
  case 16:
    gMode = 1;
    break;
  case 32:
    gMode = 2;
    break;
  case 48: 
    gMode = 3;
    break;
  }// end switch
delay(250);
}// end gameMode.

////////////////////////////////////////////////////////////////////////////
//// SELECTDIFFICULTY /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void selectDifficulty()
{
uint8_t difficulty = 1;
setScore(difficulty);
bool flag = true;

while(flag)
  {
  //Serial.println("Entered While");

  if( digitalRead(blueButton) == 0)
    {
    difficulty++;
    if( difficulty > 4)
      {
      difficulty = 1;
      }// end inner if
    setScore(difficulty);
    tone(toneButton, NOTE_C5 , duration);
    delay(200);
    }
    else if( digitalRead(greenButton) == 0){ flag = false; delay(200); }
    else
    {
    digitalWrite(blueLED, HIGH);
    delay(100);
    digitalWrite(blueLED, LOW);
    delay(100);
    }
  }// end while

setScore(0);

switch( difficulty)
  {
  case 1:
    gDifficulty = 8;
    break;
  case 2:
    gDifficulty = 14;
    break;
  case 3: 
    gDifficulty = 20;
    break;
  case 4:
    gDifficulty = 31;
    break;
  }// end switch
delay(250);
}// end selectDifficulty.

//////////////////////////////////////////////////////////////////////////
//// STARTINGSEQUENCE ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void startingSequence()
{
for( int index = 0; index< 10; index++)
   {
   tone(toneButton, notesArray[index], 150 );
   digitalWrite(LEDArray[index], HIGH);
   delay(150);
   digitalWrite(LEDArray[index], LOW);
   }
}

//////////////////////////////////////////////////////////////////////////
//// SETSCORE ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int setScore(uint8_t score)
{
// Set the Scorce to the the Seven Segment display.
for ( int index = 0; index < 6; index++)
    {
    digitalWrite(BCDPins[index], bitRead(score, index));
    }
}

//////////////////////////////////////////////////////////////////////////
//// START ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void start()
{
bool flag = true;

while( flag )
  {
    if( digitalRead(greenButton) == 0) flag = false;
    else
    {
    digitalWrite(greenLED, HIGH);
    delay(200);
    digitalWrite(greenLED, LOW);
    delay(200);
    }
  }
}

//////////////////////////////////////////////////////////////////////////
//// RESTART ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void restart()
{
gFlag = true;
gSeqCount = 1;
setScore(0);

for ( int index = 6; index < 10; index++)
    {
    tone(toneButton, notesArray[index], 150 );
    digitalWrite(LEDArray[index], HIGH);
    delay(150);
    digitalWrite(LEDArray[index], LOW);
    }

switch( gMode )
  {
  case 1:
    initializeRandomColorSequence();
    break;
  case 2:
    playerSays();
    break;
  case 3:
    gMultiCount = 2;
    multiPlayerFirstTurn();
    break;
  }
}// end restart.

//////////////////////////////////////////////////////////////////////////
//// CHECKSEQUENCE ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool checkSequence(char genericArray[32][7])
{
// Serial.println("Entered Check");
// Compare if the sequence giving by the "user" == the sequence generated by the complier.
// ! (not) operator give 0 in strcmpl
if( !strcmp ( userInputSequence[count], genericArray[count]) )
  {
    count++;
    turns++;

    //Give enough delay so that the First input doesn't get detected as the 2nd or 3rd or nth input.
    delay(200);
    return true;
  }
  else
  {
  // Game is over if the sequence doesn't match.
  gameOver();

  // Set the flag to false;
  gFlag = false;
  turns = 100;
  return false;
  }
}

//////////////////////////////////////////////////////////////////////////
//// SHOWSEQUENCE ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void showSequence(char genericArray[31][7])
{
for( int index = 0; index < gSeqCount; index++)
  {
  char col[0];
  strcpy( col, genericArray[index] );
  delay(200);
  switch( col[0] )
    {
    case 'R':
    digitalWrite(redLED, HIGH);
    delay(200);
    digitalWrite(redLED, LOW);
    tone(toneButton, NOTE_E4 , duration);
    break;

    case 'G':
    digitalWrite(greenLED, HIGH);
    delay(200);
    digitalWrite(greenLED, LOW);
    tone(toneButton, NOTE_G4 , duration);
    break;

    case 'B':
    digitalWrite(blueLED, HIGH);
    delay(200);
    digitalWrite(blueLED, LOW);
    tone(toneButton, NOTE_G3 , duration);
    break;

    case 'Y':
    digitalWrite(yellowLED, HIGH);
    delay(200);
    digitalWrite(yellowLED, LOW);
    tone(toneButton, NOTE_C4 , duration);
    break;
    }
  }
}

//////////////////////////////////////////////////////////////////////////
//// GAMEOVER ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void gameOver()
{
tone(toneButton, 41, 1500);
digitalWrite(redLED, HIGH);
digitalWrite(blueLED, HIGH);
digitalWrite(greenLED, HIGH);
digitalWrite(yellowLED, HIGH);
delay(1600);
digitalWrite(redLED, LOW);
digitalWrite(blueLED, LOW);
digitalWrite(greenLED, LOW);
digitalWrite(yellowLED, LOW);
}