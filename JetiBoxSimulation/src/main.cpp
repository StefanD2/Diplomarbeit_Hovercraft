#include <stdlib.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include <Arduino.h>

#include <JetiModes.hpp>
#define JETI_MODE ARDUINO_UNO
#include <Jeti.hpp>

#include <PJONSoftwareBitBang.h>

#define NUMBER_OF_MSG 1 // 2 doesn't work

PJONSoftwareBitBang bus(45);

Jeti jeti1 = Jeti();

void onMsg(String string);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info){};

void setup()
{
  bus.strategy.set_pin(10);
  bus.set_receiver(receiver_function);
  bus.begin();

  jeti1.init();

  sei(); // enable interrupt

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  bus.send(44, "hi", 2);
}

bool led = 0;

void loop()
{
  bus.update();
  jeti1.loop();
  if (jeti1.isNewMsg())
  {
    onMsg(jeti1.getMsg());
  }
}

void onMsg(String string)
{
  led = !led;
  digitalWrite(LED_BUILTIN, led);
  bus.send(44, string.c_str(), string.length());
}