#include <SPI.h>
#include <mcp2515.h>

#define CAN_ID_CONTROL_MOTORS_SERVOS 0xC0

#define CAN_ID_INFOS_LOWER_CONTROLLER 0xC1
#define CAN_ID_INFOS_BACK_CONTROLLER 0xC3

struct can_frame canMsg;
struct can_frame canMsg_r;
MCP2515 mcp2515(7);

void setup(){
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  Serial.begin(115200);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();
  canMsg.can_id=CAN_ID_CONTROL_MOTORS_SERVOS;
  canMsg.can_dlc=3;
}

unsigned long last_update=0;

void loop(){
  if (millis()-last_update>50){
  canMsg.data[0]=min(analogRead(A0)>>2,64); // unterer Motor
  canMsg.data[1]=analogRead(A1)>>2; //hinterer Motor
  canMsg.data[2]=map(analogRead(A2)>>2,23,88,0,255); //Lenkung
  mcp2515.sendMessage(&canMsg);
  last_update=millis();
  }

  if (mcp2515.readMessage(&canMsg_r) == MCP2515::ERROR_OK) {
    if ((canMsg_r.can_id==CAN_ID_INFOS_LOWER_CONTROLLER)&&(canMsg_r.can_dlc==6)){
      int spannung = canMsg_r.data[0]<<8| canMsg_r.data[1];
      Serial.println(spannung/100.0);
    }
  }
}