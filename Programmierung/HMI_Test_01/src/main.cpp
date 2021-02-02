#include <SPI.h>
#include <mcp2515.h>
#include "Nextion.h"

//#define NEXTION_ENABLED

#define NEXTION_ENABLED
#ifdef NEXTION_ENABLED

NexText disp_drz_u = NexText(0,8,"drzunt");
NexButton but_01 = NexButton(0,2,"b00");
NexText disp_temp_u = NexText(0,1,"tempunt");
NexText disp_gas_u = NexText(0,13,"gasunt");
NexText disp_spg_u = NexText(0,15,"spgunt");

NexText disp_drz_h = NexText(0,24,"drzhint");
NexText disp_temp_h = NexText(0,25,"temphint");
NexText disp_gas_h = NexText(0,29,"gashint");
NexText disp_spg_h = NexText(0,31,"spghint");


char buffer[100] = {0};
#endif


#define CAN_ID_CONTROL_MOTORS_SERVOS 0xC0

#define CAN_ID_INFOS_LOWER_CONTROLLER 0xC1
#define CAN_ID_INFOS_BACK_CONTROLLER 0xC3

NexTouch *nex_listen_list[] = 
{
    &but_01,
    NULL
};

struct can_frame canMsg;
struct can_frame canMsg_r;
MCP2515 mcp2515(7);

void but01_push(void *ptr){
  disp_temp_u.setText("LOW");
};

void but01_pop(void *ptr){
  disp_temp_u.setText("HIGH");
};

void setup(){
  pinMode(LED_BUILTIN,OUTPUT);
  #ifdef NEXTION_ENABLED
  Serial.begin(9600);
  nexInit();
  but_01.attachPush(but01_push);
  but_01.attachPop(but01_pop);
  delay(500);
  // Serial.print("baud=38400");
  // Serial.write(0xff);
  // Serial.write(0xff);
  // Serial.write(0xff);
  // Serial.end();
  // Serial.begin(38400);
  disp_temp_u.setText("Test");
  #endif
}

void loop(){
  //memset(buffer, 0, sizeof(buffer));
  //itoa(120, buffer, 10);
  //disp_temp_u.setText("Test");
  nexLoop(nex_listen_list);
  }