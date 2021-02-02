#include <SPI.h>
#include <mcp2515.h>
#include "Nextion.h"

//#define NEXTION_ENABLED

#ifdef NEXTION_ENABLED

NexText disp_drz_u = NexText(0,8,"drzunt");
NexText disp_temp_u = NexText(0,9,"tempunt");
NexText disp_gas_u = NexText(0,13,"gas_u");
NexText disp_spg_u = NexText(0,15,"spg_u");

NexText disp_drz_h = NexText(0,24,"drz_h");
NexText disp_temp_h = NexText(0,25,"temp_h");
NexText disp_gas_h = NexText(0,29,"gas_h");
NexText disp_spg_h = NexText(0,31,"spg_h");


char buffer[100] = {0};
#endif


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
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();
  canMsg.can_id=CAN_ID_CONTROL_MOTORS_SERVOS;
  canMsg.can_dlc=3;

  #ifdef NEXTION_ENABLED
  Serial.begin(9600);
  nexInit();
  delay(500);
  // Serial.print("baud=38400");
  // Serial.write(0xff);
  // Serial.write(0xff);
  // Serial.write(0xff);
  // Serial.end();
  // Serial.begin(38400);
  #endif
}

unsigned long last_update=0;
int test=0;
void loop(){
  if (millis()-last_update>50){
  canMsg.data[0]=min(analogRead(A0)>>2,128); // unterer Motor
  canMsg.data[1]=analogRead(A1)>>2; //hinterer Motor
  canMsg.data[2]=map(analogRead(A2)>>2,23,88,0,255); //Lenkung
  mcp2515.sendMessage(&canMsg);
  last_update=millis();
  #ifdef NEXTION_ENABLED
  memset(buffer, 0, sizeof(buffer));
  itoa(120, buffer, 10);
  disp_temp_u.setText(buffer);
  #endif
  }

  if (mcp2515.readMessage(&canMsg_r) == MCP2515::ERROR_OK) {
    if ((canMsg_r.can_id==CAN_ID_INFOS_LOWER_CONTROLLER)&&(canMsg_r.can_dlc==6)){ //Infos from lower motor controller received
      int spannung = (canMsg_r.data[0]<<8)| canMsg_r.data[1];
      int drehzahl = (canMsg_r.data[4]<<8)|canMsg_r.data[5];
      char temperatur = canMsg_r.data[2];
      char percent = canMsg_r.data[3];
      //Serial.println(spannung/100.0);
      #ifdef NEXTION_ENABLED
        disp_drz_u.setText(String(drehzahl).c_str());
        disp_temp_u.setText(String(temperatur).c_str());
      #endif

    }
    
  }
}