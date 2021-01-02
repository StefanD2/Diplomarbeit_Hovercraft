#include <SPI.h>
#include <mcp2515.h>

#define CAN_ID_CONTROL_MOTORS_SERVOS 0xC0

struct can_frame canMsg;
MCP2515 mcp2515(7);

void setup(){
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();
  canMsg.can_id=CAN_ID_CONTROL_MOTORS_SERVOS;
  canMsg.can_dlc=3;
}

void loop(){
  canMsg.data[0]=analogRead(A1)>>2;
  canMsg.data[1]=analogRead(A2)>>2;
  canMsg.data[2]=analogRead(A3)>>2;
  delay(5);
  mcp2515.sendMessage(&canMsg);
  delay(100);
}