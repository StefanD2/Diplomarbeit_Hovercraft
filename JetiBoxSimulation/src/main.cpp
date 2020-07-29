#include <stdlib.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include <Arduino.h>
#include <Streaming.h>

#include "Jeti.hpp"
#include "Jeti_ATmega_Serial1.hpp"

#define NUMBER_OF_MSG 1 // 2 doesn't work

Jeti_ATmega_Serial1 jeti1 = Jeti_ATmega_Serial1();

void onMsg(String string);

void setup()
{

  jeti1.init();

  sei(); // enable interrupt

  Serial.begin(921600);
  Serial << "START" << endl;
}

void loop()
{
  jeti1.loop();
  if (jeti1.isNewMsg())
  {
    String s = jeti1.getMsg();
    onMsg(s);
    //Serial << s << endl;
  }
}

unsigned long lastTime = 0;

unsigned long startTime = 0;
unsigned long directionTime = 0;
unsigned long endTime = 0;

bool changedDir = false;
bool sollDir = true; // 0 ... left, 1 .. right

void onMsg(String string)
{
  if (string.indexOf(changedDir ? "MASTER" : "Rotation") == -1)
  {
    jeti1.send(false, false, changedDir, !changedDir, NUMBER_OF_MSG);
  }
  else
  {
    if (string.indexOf("Rotation") != -1)
    {
      if (string.indexOf(sollDir ? "RIGHT" : "LEFT") == -1)
      {
        jeti1.send(!sollDir, sollDir, false, false, NUMBER_OF_MSG);
      }
      else
      {
        changedDir = true;
        directionTime = millis();
        Serial << "change direction to " << (sollDir ? "RIGHT" : "LEFT") << " " << (directionTime - startTime) << endl;
        Serial.flush();
      }
    }
    else
    {
      endTime = millis();
      Serial << "main menu " << (endTime - startTime) << endl;
      Serial.flush();
    }
  }
}