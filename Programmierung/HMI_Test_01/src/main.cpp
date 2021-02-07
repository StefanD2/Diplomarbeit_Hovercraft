#include <SPI.h>
#include <mcp2515.h>
#include "Nextion.h"

#define CAN_ID_CONTROL_MOTORS_SERVOS 0xC0

#define CAN_ID_INFOS_LOWER_CONTROLLER 0xC1
#define CAN_ID_INFOS_BACK_CONTROLLER 0xC3


#define NEXTION_ENABLED
#ifdef NEXTION_ENABLED

//Main page nextion objects
NexText disp_drz_u = NexText(0,3,"drzunt");
NexText disp_temp_u = NexText(0,2,"tempunt");
NexText disp_gas_u = NexText(0,24,"gasunt");
NexText disp_spg_u = NexText(0,4,"spgunt");

NexText disp_drz_h = NexText(0,10,"drzhint");
NexText disp_temp_h = NexText(0,9,"temphint");
NexText disp_gas_h = NexText(0,29,"gashint");
NexText disp_spg_h = NexText(0,11,"spghint");

NexButton disp_but00_settings = NexButton(0,22,"b00");
NexButton disp_but01_infos = NexButton(0,23,"b01");

//info page nextion objects
NexText disp_atemp_01 = NexText(2,14,"temp01");
NexText disp_atemp_02 = NexText(2,15,"temp02");
NexText disp_atemp_03 = NexText(2,16,"temp03");
NexText disp_atemp_04 = NexText(2,17,"temp04");
NexText disp_atemp_05 = NexText(2,18 ,"temp05");
NexText disp_atemp_06 = NexText(2,19,"temp06");
NexText disp_atemp_07 = NexText(2,20,"temp07");
NexText disp_atemp_08 = NexText(2,21,"temp08");
NexButton disp_but20_back = NexButton(2,1,"b20");


char buffer[100] = {0};
int nextion_page=0;

NexTouch *nex_listen_list[] = 
{
    &disp_but00_settings,
    &disp_but01_infos,
    &disp_but20_back,
    NULL
};

void but00_settings_push(void *ptr){
  nextion_page=1;
};
void but01_infos_push(void *ptr){
  nextion_page=2;
};
void but20_back(void *ptr){
  nextion_page=0;
}


#endif


struct can_frame canMsg;
struct can_frame canMsg_r;
MCP2515 mcp2515(7);





void setup(){
  pinMode(LED_BUILTIN,OUTPUT);
  #ifdef NEXTION_ENABLED
  Serial.begin(9600);
  nexInit();
  disp_but01_infos.attachPush(but01_infos_push);
  disp_but00_settings.attachPush(but00_settings_push);
  disp_but20_back.attachPush(but20_back);
  delay(500);
  Serial.print("baud=38400");
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.end();
  Serial.begin(38400);
  delay(500);
  #endif
}

int i = 1050;
int temp = 10;

void loop(){
  switch (nextion_page){
    case 0:
      disp_spg_u.setText(String(i/100.0).c_str());
      disp_temp_h.setText(String(temp).c_str());
      break;
    case 2:
      disp_atemp_05.setText(String(temp).c_str());
    break;
  }

  i+=110;
  temp+=10;
  delay(500);
  //memset(buffer, 0, sizeof(buffer));
  //itoa(120, buffer, 10);
  //disp_temp_u.setText("Test");
  #ifdef NEXTION_ENABLED
  nexLoop(nex_listen_list);
  #endif
  }