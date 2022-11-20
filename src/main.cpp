#include <Arduino.h>
#include <stdint.h>
#include "pitches.h"

#define DEBUG

int analogPin = A0; // For Random

/*Pins for buttons, buzzer and leds*/
const int buttonPins[] = {2, 3, 4};
const int ledPins[] = {5, 6, 7};
const int buzzerPin = 8;

/*Sounds*/
const int sounds[] = {NOTE_C4, NOTE_D4, NOTE_A4};

/*Memory for input and for generation*/
uint8_t playsMem[150];
uint8_t playsindex = 0;

uint8_t inputMem[150];
uint8_t inputIndex = 0;

bool finnishedInput = false;

int8_t pressedButton = 0;
bool debounced = true;

/*Function Protoptypes*/
void toggle();
uint8_t getRandomNumber(uint8_t low, uint8_t up);
void clearLED();
int8_t getPressedButton();
void setup();
int game();
void gameOver();
void loop();

/*Functions*/

uint8_t getRandomNumber(uint8_t low, uint8_t up)
{
  uint8_t ret = 0;
  ret = (uint8_t)random(low, up + 1);
  return ret;
}

void clearLED()
{
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++)
  {

    digitalWrite(ledPins[i], 0);
  }
}

void soundAndBlinkOnce(int sound, int pinLEd, int durationSound, int durationBlink)
{
  tone(buzzerPin, sound, durationSound);
  digitalWrite(pinLEd, 1);
  delay(durationBlink);
  digitalWrite(pinLEd, 0);
}

int8_t getPressedButton()
{
  int bitmask = 0;

  for (int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++)
  {
    if (digitalRead(buttonPins[i]) == HIGH)
    {
      bitmask = bitmask | (1 << i);
    }
  }

  switch (bitmask)
  {
  case (0b00000000):
    return 0;
  case (0b00000001):
    return 1;
  case (0b00000010):
    return 2;
  case (0b00000100):
    return 3;
  //... Up to 7 Buttons
  default:
    return -1;
  }
}

void setup()
{

  //Serial.begin(9600); // open the serial port at 9600 bps:
  randomSeed(analogRead(4));

  playsMem[0] = getRandomNumber(1, 3); // Init Game
  playsindex++;

  for (int i = 0; i < sizeof(buttonPins) / sizeof(buttonPins[0]); i++)
  {
    pinMode(buttonPins[i], INPUT);
  }
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], 0);
  }

  soundAndBlinkOnce(sounds[playsMem[0] - 1], ledPins[playsMem[0] - 1], 50, 600);
}

int game()
{
  int i = 0;
  for (i = 0; i < playsindex; i++)
  {
    if (playsMem[i] != inputMem[i])
    {
      return -1; //Failed to remember
    }
  }

  delay(500);

  if(playsindex+1 == sizeof(playsMem)/sizeof(playsMem[0])){ //If memory of player is to good
    playsindex = 1;
  }
  

  for (i = 0; i < playsindex; i++)
  {
    
    soundAndBlinkOnce(sounds[playsMem[i] - 1], ledPins[playsMem[0] - 1], 50, 600);

  }
  /*New number*/
  playsMem[i] = getRandomNumber(1, 3);
  playsindex++;
  delay(600);
  soundAndBlinkOnce(sounds[playsMem[i] - 1], ledPins[playsMem[0] - 1], 50, 600);

  // Clear all
  clearLED();
  return 0;
}

void gameOver()
{
  digitalWrite(ledPins[getRandomNumber(0, 2)], 1);
  tone(buzzerPin, NOTE_G5, 500);
  tone(buzzerPin, NOTE_A4, 500);
  tone(buzzerPin, NOTE_B1, 800);
  while (1)
  {
    delay(100);
    digitalWrite(ledPins[getRandomNumber(0, 2)], 1);
    delay(100);
    digitalWrite(ledPins[getRandomNumber(0, 2)], 0);
  }
}

void loop()
{

 



  delay(200);
  pressedButton = getPressedButton();
  if (pressedButton > 0 && debounced == true)
  {
    
    soundAndBlinkOnce(sounds[pressedButton - 1], ledPins[pressedButton - 1], 50, 50);
    debounced = false;
    inputMem[inputIndex] = pressedButton;
    inputIndex++;

    if (inputIndex == playsindex)
    {
      finnishedInput = true;
    }
  }

  if ((pressedButton == 0) && debounced == false)
  {

    clearLED();
    debounced = true;
  }

  if (finnishedInput == true && debounced == true)
  {
    delay(200);
    if (game() < 0)
    {
      gameOver();
    }
    else
    {
      inputIndex = 0;
      finnishedInput = false;
    }
  }
}