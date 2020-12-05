#include <stdlib.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include <Arduino.h>

#include <JetiModes.hpp>
#define JETI_MODE ARDUINO_MEGA_1
#include <Jeti.hpp>

#include <Streaming.h>

JetiBase *jeti = new Jeti_ATmega_Serial1(); // access object via pointer (this allows switching between JETI_MODES with minimal code cahnges)

void setup()
{
  Serial.begin(115200);
  Serial.println("start");

  jeti->init();

  sei(); // enable interrupt

  Serial.println("setup done");

}

void loop()
{
  jeti->loop();
  if (jeti->isNewMsg())
  {
    jetiTelemetry_t tel = JetiBase::getTelemetry(jeti->getMsg());
    Serial << "mode " << tel.mode << " " << tel.percent << "% " << tel.rpm << "rpm " << tel.voltage << "V " << tel.temperature << "°C" << endl;
  }
}
