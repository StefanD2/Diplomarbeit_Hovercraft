#include <Arduino.h>
#include <PJONSoftwareBitBang.h>
#include <Streaming.h>


PJONSoftwareBitBang bus(44);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info)
{
  for (uint16_t i = 0; i < length; i++)
    Serial << (char)payload[i];
  Serial << endl;
};

void setup()
{
  Serial.begin(115200);

  bus.strategy.set_pin(10);
  bus.set_receiver(receiver_function);
  bus.begin();
  Serial << "hallo" << endl;
}

void loop()
{
  bus.update();
  bus.receive(10000); // in micros
};