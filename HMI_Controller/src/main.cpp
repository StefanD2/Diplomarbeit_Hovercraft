#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

#define DATA_REGLER_UNTEN 0xF5
MCP2515 mcp2515(7);

struct can_frame canMsg;
void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();
  // put your setup code here, to run once:
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if ((canMsg.can_id==DATA_REGLER_UNTEN)&&(canMsg.can_dlc==6)){
        Serial.print("Spannung:");
        int spannung = canMsg.data[0]<<8| canMsg.data[1];
        Serial.print(spannung/100.0);
        Serial.println("V");
        Serial.print("Prozent:");
        Serial.print(canMsg.data[3]);
        Serial.println("%");
        int drehzahl = canMsg.data[4]<<8|canMsg.data[5];
        Serial.print(drehzahl);
        Serial.println("rpm");
        Serial.print("Temperatur:");
        Serial.print(canMsg.data[2]);
        Serial.println("°C");
        Serial.println("------------------------------------");

    }
  }
}