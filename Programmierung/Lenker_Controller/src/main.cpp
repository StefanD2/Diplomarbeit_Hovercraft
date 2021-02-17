#include <SPI.h>
#include <mcp2515.h>
#include "Nextion.h"
#include "my_TinyGPS++.h"
#include <SoftwareSerial.h>

static const int RXPin = 8, TXPin = 9;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

//This custom version of delay() ensures that the gps object
//is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

#define CAN_ID_CONTROL_MOTORS_SERVOS 0xC0
#define CAN_ID_INFOS_LOWER_CONTROLLER 0xC1
#define CAN_ID_INFOS_BACK_CONTROLLER 0xC3
#define CAN_ID_BATTERY_TEMPS_LEFT 0xE0
#define CAN_ID_BATTERY_TEMPS_RIGHT 0xE1




#define NEXTION_ENABLED


int nextion_page=0;
uint32_t max_power_lower=255;
uint32_t max_power_back=255;
uint32_t steering_offset=0;

#ifdef NEXTION_ENABLED

//Main page nextion objects
NexText disp_gps_speed = NexText(0,15,"gpsspeed");

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
//akku infos
NexText disp_atemp_01 = NexText(2,14,"temp01");
NexText disp_atemp_02 = NexText(2,15,"temp02");
NexText disp_atemp_03 = NexText(2,16,"temp03");
NexText disp_atemp_04 = NexText(2,17,"temp04");
NexText disp_atemp_05 = NexText(2,18 ,"temp05");
NexText disp_atemp_06 = NexText(2,19,"temp06");
NexText disp_atemp_07 = NexText(2,20,"temp07");
NexText disp_atemp_08 = NexText(2,21,"temp08");
NexButton disp_but20_back = NexButton(2,1,"b20");

//gps infos
NexText disp_gpsinf_sats = NexText(2,34,"gpssats");
NexText disp_gpsinf_speed = NexText(2,41,"gpsspeed");
NexText disp_gpsinf_course = NexText(2,40,"gpscorse");
NexText disp_gpsinf_lon = NexText(2,36,"gpslon");
NexText disp_gpsinf_lat = NexText(2,37,"gpslat");
NexText disp_gpsinf_height = NexText(2,35,"gpsalt");

//settings page nextion objects
NexSlider disp_slid_lenk_offset = NexSlider(1,3,"h1");
NexSlider disp_slid_maxbackpower = NexSlider(1,2,"h0");
NexSlider disp_slid_maxdownpower = NexSlider(1,4,"h2");
NexButton disp_but10_back = NexButton(1,1,"b10");


char buffer[100] = {0};


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
void but10_back(void *ptr){
  nextion_page=0;
}
void slid_lenk_offset(void *ptr){
  disp_slid_lenk_offset.getValue(&steering_offset);
}
void slid_maxbackpower(void *ptr){
  disp_slid_maxbackpower.getValue(&max_power_back);
}
void slid_maxdownpower(void *ptr){
  disp_slid_maxdownpower.getValue(&max_power_lower);
}


#endif



struct can_frame canMsg;
struct can_frame canMsg_r;
MCP2515 mcp2515(7);

void setup(){
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);

  //Initialize CAN-Bus shield
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();
  canMsg.can_id=CAN_ID_CONTROL_MOTORS_SERVOS;
  canMsg.can_dlc=3;

  //initialize NEXTION display
  #ifdef NEXTION_ENABLED
  Serial.begin(9600);
  nexInit();

  //attach interrupts to ISR
  disp_but01_infos.attachPush(but01_infos_push);
  disp_but00_settings.attachPush(but00_settings_push);
  disp_but10_back.attachPush(but10_back);
  disp_but20_back.attachPush(but20_back);
  disp_slid_lenk_offset.attachPop(slid_lenk_offset);
  disp_slid_maxbackpower.attachPop(slid_maxbackpower);
  disp_slid_maxdownpower.attachPop(slid_maxdownpower);
  delay(500);

  //change baudrate of nextion display from 9600 to 38400 baud
  Serial.print("baud=38400");
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.end(); //end serial with 9600 baud
  Serial.begin(38400); //restart it with 38400 baud
  delay(500);
  #endif

  ss.begin(9600); //initialize SoftwareSerial for GPS module
}

unsigned long last_update=0;
unsigned long last_gps_update=0;
int test=0;


void loop(){
  if (millis()-last_update>50){
  canMsg.data[0]=min(analogRead(A0)>>2,(int)max_power_lower); // unterer Motor
  canMsg.data[1]=min(analogRead(A1)>>2,(int)max_power_back); //hinterer Motor
  canMsg.data[2]=(int)(steering_offset-10)+map(analogRead(A2)>>2,23,88,0,255); //Lenkung
  mcp2515.sendMessage(&canMsg);
  last_update=millis();
  }
  #ifdef NEXTION_ENABLED
  if (millis()-last_gps_update>500){
    if (nextion_page==0){
      disp_gps_speed.setText(String(gps.speed.kmph()/1.0).c_str());
    } else if (nextion_page==2){
      disp_gpsinf_sats.setText(String(gps.satellites.value()).c_str());
      disp_gpsinf_speed.setText(String(gps.speed.kmph()).c_str());
      disp_gpsinf_course.setText(String(gps.speed.kmph()).c_str());
      disp_gpsinf_lon.setText(String(gps.location.lng()).c_str());
      disp_gpsinf_lat.setText(String(gps.location.lat()).c_str());
      disp_gpsinf_height.setText(String(gps.altitude.meters()).c_str());
    }
    last_gps_update=millis();
  }
  disp_gps_speed.setText(String(gps.speed.kmph()/1.0).c_str());
  if (mcp2515.readMessage(&canMsg_r) == MCP2515::ERROR_OK) {
    if ((canMsg_r.can_id==CAN_ID_INFOS_LOWER_CONTROLLER || (canMsg_r.can_id==CAN_ID_INFOS_BACK_CONTROLLER))&&(canMsg_r.can_dlc==6)&&nextion_page==0){ //Infos from lower motor controller received
      int spannung = (canMsg_r.data[0]<<8)| canMsg_r.data[1];
      int drehzahl = (canMsg_r.data[4]<<8)|canMsg_r.data[5];
      int temperatur = canMsg_r.data[2];
      int percent = canMsg_r.data[3];
  
        if (canMsg_r.can_id==CAN_ID_INFOS_LOWER_CONTROLLER){
          disp_drz_u.setText(String(drehzahl).c_str());
          disp_temp_u.setText(String(temperatur).c_str());
          disp_gas_u.setText(String(percent).c_str());
          disp_spg_u.setText(String(spannung/100.0).c_str());
        } else{
          disp_drz_h.setText(String(drehzahl).c_str());
          disp_temp_h.setText(String(temperatur).c_str());
          disp_gas_h.setText(String(percent).c_str());
          disp_spg_h.setText(String(spannung/100.0).c_str());
        }
    } else if ((canMsg_r.can_id==CAN_ID_BATTERY_TEMPS_LEFT || (canMsg_r.can_id==CAN_ID_BATTERY_TEMPS_RIGHT))&&(canMsg_r.can_dlc==8)&&nextion_page==2){
        if (canMsg_r.can_id==CAN_ID_BATTERY_TEMPS_LEFT){
            disp_atemp_01.setText(String((canMsg_r.data[0]<<8)| canMsg_r.data[1]).c_str());
            disp_atemp_02.setText(String((canMsg_r.data[2]<<8)| canMsg_r.data[3]).c_str());
            disp_atemp_03.setText(String((canMsg_r.data[4]<<8)| canMsg_r.data[5]).c_str());
            disp_atemp_04.setText(String((canMsg_r.data[6]<<8)| canMsg_r.data[7]).c_str());
    } else{
            disp_atemp_05.setText(String((canMsg_r.data[0]<<8)| canMsg_r.data[1]).c_str());
            disp_atemp_06.setText(String((canMsg_r.data[2]<<8)| canMsg_r.data[3]).c_str());
            disp_atemp_07.setText(String((canMsg_r.data[4]<<8)| canMsg_r.data[5]).c_str());
            disp_atemp_08.setText(String((canMsg_r.data[6]<<8)| canMsg_r.data[7]).c_str());
      }
    
    }
  }
  nexLoop(nex_listen_list);
  #endif
  smartDelay(0);
}