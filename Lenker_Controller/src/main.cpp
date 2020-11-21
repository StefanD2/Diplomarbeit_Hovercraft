#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(7);

void setup(){
  pinMode(A6,INPUT);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();
  canMsg.can_id=0xF5;
  canMsg.can_dlc=2;
  canMsg.data[0]=0x00;
  Serial.begin(115200);
}

void loop(){
  canMsg.data[1]=analogRead(A6)>>2;
  mcp2515.sendMessage(&canMsg);
  delay(20);
}