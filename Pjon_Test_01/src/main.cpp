#include <Arduino.h>
#include <PJONSoftwareBitBang.h>
#include <Streaming.h>

#define NANO 45
#define UNO1 46 // orginal uno
#define UNO2 44

#define NAME "UNO1"
#define ID UNO1

PJONSoftwareBitBang bus(ID);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info)
{
  
    Serial.print("Header: ");
  Serial.print(packet_info.header, BIN);
  // If packet formatted for a shared medium
  if(packet_info.header & PJON_MODE_BIT) {
    Serial.print(" Receiver bus id: ");
    Serial.print(packet_info.receiver_bus_id[0]);
    Serial.print(packet_info.receiver_bus_id[1]);
    Serial.print(packet_info.receiver_bus_id[2]);
    Serial.print(packet_info.receiver_bus_id[3]);
    Serial.print(" Receiver id: ");
    Serial.print(packet_info.receiver_id);
    // If sender info is included
    if(packet_info.header & PJON_TX_INFO_BIT) {
      Serial.print(" Sender bus id: ");
      Serial.print(packet_info.sender_bus_id[0]);
      Serial.print(packet_info.sender_bus_id[1]);
      Serial.print(packet_info.sender_bus_id[2]);
      Serial.print(packet_info.sender_bus_id[3]);
    }
  }
  // If sender device id is included
  if(packet_info.header & PJON_TX_INFO_BIT) {
    Serial.print(" Sender id: ");
    Serial.print(packet_info.sender_id);
  }
  // Payload Length
  Serial.print(" Length: ");
  Serial.print(length);
  // If port id is included
  if(packet_info.header & PJON_PORT_BIT) {
    Serial.print(" Port bit: ");
    Serial.print(packet_info.port);
  }
  Serial.println();
};

void setup()
{
  Serial.begin(115200);

  bus.strategy.set_pin(10);
  bus.set_receiver(receiver_function);
  bus.begin();
#if ID == NANO
  bus.send_repeatedly(UNO2, NAME, 4, 1000000); // Send B to device 44 every second
#elif ID == UNO1
  bus.send_repeatedly(NANO, NAME, 4, 1000000); // Send B to device 44 every second
  bus.send_repeatedly(UNO2, NAME, 4, 1000000); // Send B to device 44 every second
#elif ID == UNO2
  bus.send_repeatedly(NANO, NAME, 4, 1000000); // Send B to device 44 every second
#endif
  Serial << "hallo" << endl;
}

void loop()
{
  bus.update();
  bus.receive(10000); // in micros
  static unsigned long lastSend;
  if (1 || millis() - lastSend > 500)
  {
    String s = NAME + String(" ") + String(millis());
#if ID == NANO
    Serial << bus.send(UNO2, s.c_str(), s.length());
#elif ID == UNO1
    Serial << bus.send(UNO2, s.c_str(), s.length());
    Serial << bus.send(NANO, s.c_str(), s.length());
#elif ID == UNO2
    Serial << bus.send(NANO, s.c_str(), s.length());
#endif
    lastSend = millis();
  }
};