#include <SPI.h>
#include <mcp2515.h>
#include "Nextion.h"
#include "my_TinyGPS++.h"
#include <SoftwareSerial.h>

#define CAN_ID_CONTROL_MOTORS_SERVOS 0xC0
#define CAN_ID_INFOS_LOWER_CONTROLLER 0xC1
#define CAN_ID_INFOS_BACK_CONTROLLER 0xC3
#define CAN_ID_BATTERY_TEMPS 0xE0

static const int RXPin = 8, TXPin = 9;
static const uint32_t GPSBaud = 9600;

uint32_t max_power_lower = 255;
uint32_t max_power_back = 255;
uint32_t steering_offset = 0;

#define MIN_VALUE_A0 190 //Analogue value of thumb throttle connected to A0 when not pressed
#define MAX_VALUE_A0 862 //Analogue value of thumb throttle connected to A0 when fully pressed
#define MIN_VALUE_A1 185 //Analogue value of thumb throttle connected to A1 when not pressed
#define MAX_VALUE_A1 865 //Analogue value of thumb throttle connected to A1 when fully pressed

TinyGPSPlus gps; //TinyGPS++ object

SoftwareSerial ss(RXPin, TXPin); //serial connection the GPS device

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

#define NEXTION_ENABLED

#ifdef NEXTION_ENABLED

int nextion_page = 0; //screen number currently displayed on nextion

// ---------nextion objects for main page ----- 
NexText disp_gps_speed = NexText(0, 15, "gpsspeed");

NexText disp_drz_u = NexText(0, 3, "drzunt");
NexText disp_temp_u = NexText(0, 2, "tempunt");
NexText disp_gas_u = NexText(0, 24, "gasunt");
NexText disp_spg_u = NexText(0, 4, "spgunt");

NexText disp_drz_h = NexText(0, 10, "drzhint");
NexText disp_temp_h = NexText(0, 9, "temphint");
NexText disp_gas_h = NexText(0, 29, "gashint");
NexText disp_spg_h = NexText(0, 11, "spghint");

NexButton disp_but00_settings = NexButton(0, 22, "b00");
NexButton disp_but01_infos = NexButton(0, 23, "b01");

// ---- nextion objects for info page --------- 
//battery infos
NexText disp_atemp_01 = NexText(2, 14, "temp01");
NexText disp_atemp_02 = NexText(2, 15, "temp02");
NexText disp_atemp_03 = NexText(2, 16, "temp03");
NexText disp_atemp_04 = NexText(2, 17, "temp04");
NexText disp_atemp_05 = NexText(2, 18, "temp05");
NexText disp_atemp_06 = NexText(2, 19, "temp06");
NexText disp_atemp_07 = NexText(2, 20, "temp07");
NexText disp_atemp_08 = NexText(2, 21, "temp08");
NexButton disp_but20_back = NexButton(2, 1, "b20");

//gps infos
NexText disp_gpsinf_sats = NexText(2, 34, "gpssats");
NexText disp_gpsinf_speed = NexText(2, 41, "gpsspeed");
NexText disp_gpsinf_course = NexText(2, 40, "gpscorse");
NexText disp_gpsinf_lon = NexText(2, 36, "gpslon");
NexText disp_gpsinf_lat = NexText(2, 37, "gpslat");
NexText disp_gpsinf_height = NexText(2, 35, "gpsalt");

// --------  nextion objects for settings page  ---------
NexSlider disp_slid_lenk_offset = NexSlider(1, 3, "h1");
NexSlider disp_slid_maxbackpower = NexSlider(1, 2, "h0");
NexSlider disp_slid_maxdownpower = NexSlider(1, 4, "h2");
NexButton disp_but10_back = NexButton(1, 1, "b10");

NexTouch *nex_listen_list[] = //list of all clickable items on the nextion display
    {
        &disp_but00_settings,
        &disp_but01_infos,
        &disp_but20_back,
        &disp_slid_lenk_offset,
        &disp_slid_maxbackpower,
        &disp_slid_maxdownpower,
        &disp_but10_back,
        NULL};

void but00_settings_push(void *ptr) //ISR for "Settings" Button on main screen
{
  disp_slid_lenk_offset.setValue(steering_offset);
  disp_slid_maxbackpower.setValue(max_power_back);
  disp_slid_maxdownpower.setValue(max_power_lower);
  nextion_page = 1;
};
void but01_infos_push(void *ptr) //ISR for "Infos" Button on main screen
{
  nextion_page = 2;
};
void but20_back(void *ptr) //ISR for "Back" button on info screen 
{
  nextion_page = 0;
}
void but10_back(void *ptr) //ISR for "Back" button on settings screen
{
  nextion_page = 0;
}
void slid_lenk_offset(void *ptr) //ISR for releasing steering offset slider
{
  disp_slid_lenk_offset.getValue(&steering_offset);
}
void slid_maxbackpower(void *ptr) //ISR for releasing maximum back power slider
{
  disp_slid_maxbackpower.getValue(&max_power_back);
}
void slid_maxdownpower(void *ptr) //ISR for releasing maximum lower power slider
{
  disp_slid_maxdownpower.getValue(&max_power_lower);
}
#endif

struct can_frame canMsg;   //CAN-Bus object used for writting to CAN-Bus
struct can_frame canMsg_r; //CAN-Bus object used for reading from CAN-Bus
MCP2515 mcp2515(7);

void setup()
{
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);

//initialize NEXTION display
#ifdef NEXTION_ENABLED
  Serial.begin(9600);
  nexInit();
  delay(100);
  //attach interrupts to ISR
  disp_but01_infos.attachPush(but01_infos_push);
  disp_but00_settings.attachPush(but00_settings_push);
  disp_but10_back.attachPush(but10_back);
  disp_but20_back.attachPush(but20_back);
  disp_slid_lenk_offset.attachPop(slid_lenk_offset);
  disp_slid_maxbackpower.attachPop(slid_maxbackpower);
  disp_slid_maxdownpower.attachPop(slid_maxdownpower);
#endif
  ss.begin(9600); //initialize SoftwareSerial for GPS module
  //Initialize CAN-Bus shield
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  canMsg.can_id = CAN_ID_CONTROL_MOTORS_SERVOS;
  canMsg.can_dlc = 3;
}

unsigned long last_update = 0;
unsigned long last_gps_update = 0;

void loop()
{
  if (millis() - last_update > 50) //Send new data to CAN-Bus every 50ms
  {
    int int_0 = map(analogRead(A0), MIN_VALUE_A0, MAX_VALUE_A0, 30, max_power_back); //map analog value of A0 between 0 and max_power_back value
    int int_1 = map(analogRead(A1), MIN_VALUE_A1, MAX_VALUE_A1, 30, max_power_lower); //map analog value of A1 between 0 and max_power_lower value
    canMsg.data[0] = min(int_0, 255); //set power value for lower motor to CAN message
    canMsg.data[1] = min(int_1, 255); //set power value of back motor to CAN message
    canMsg.data[2] = (int)(steering_offset) + map(analogRead(A2) >> 2, 23, 88, 255, 0); //calculate and set steering value for CAN message
    mcp2515.sendMessage(&canMsg); //send CAN message
    last_update = millis();
  }
#ifdef NEXTION_ENABLED
  if (millis() - last_gps_update > 500)
  { //refresh gps data on display every 500ms
    if (nextion_page == 0 && gps.speed.isValid()) //when on main page and gps data is valid
    {
      disp_gps_speed.setText(String(gps.speed.kmph() / 1.0).c_str());
    }
    else if (nextion_page == 2) //when on Info page
    {
      disp_gpsinf_sats.setText(String(gps.satellites.value()).c_str());
      disp_gpsinf_speed.setText(String(gps.speed.kmph()).c_str());
      disp_gpsinf_course.setText(String(gps.course.deg()).c_str());
      disp_gpsinf_lon.setText(String(gps.location.lng()).c_str());
      disp_gpsinf_lat.setText(String(gps.location.lat()).c_str());
      disp_gpsinf_height.setText(String(gps.altitude.meters()).c_str());
    }
    last_gps_update = millis();
  }
  if (mcp2515.readMessage(&canMsg_r) == MCP2515::ERROR_OK)
  { //check for new CAN-Bus messages
    if ((canMsg_r.can_id == CAN_ID_INFOS_LOWER_CONTROLLER || (canMsg_r.can_id == CAN_ID_INFOS_BACK_CONTROLLER)) && (canMsg_r.can_dlc == 6) && nextion_page == 0)
    {                                                            //check if message is controller telemetry and valid
      int spannung = (canMsg_r.data[0] << 8) | canMsg_r.data[1]; //recombine High & Lowbyte of voltage back to integer
      int drehzahl = (canMsg_r.data[4] << 8) | canMsg_r.data[5]; //recombine High & Lowbyte of rotations back to integer
      int temperatur = canMsg_r.data[2];
      int percent = canMsg_r.data[3];

      if (canMsg_r.can_id == CAN_ID_INFOS_LOWER_CONTROLLER)
      { //data from lower controller received
        disp_drz_u.setText(String(drehzahl).c_str());
        disp_temp_u.setText(String(temperatur).c_str());
        disp_gas_u.setText(String(percent).c_str());
        disp_spg_u.setText(String(spannung / 100.0).c_str());
      }
      else
      { //data from back controller received
        disp_drz_h.setText(String(drehzahl).c_str());
        disp_temp_h.setText(String(temperatur).c_str());
        disp_gas_h.setText(String(percent).c_str());
        disp_spg_h.setText(String(spannung / 100.0).c_str());
      }
    }
    else if ((canMsg_r.can_id == CAN_ID_BATTERY_TEMPS) && (canMsg_r.can_dlc == 8) && (nextion_page == 2))
    { //battery temperatures received and valid
      disp_atemp_01.setText(String(canMsg_r.data[0]).c_str());
      disp_atemp_02.setText(String(canMsg_r.data[1]).c_str());
      disp_atemp_03.setText(String(canMsg_r.data[2]).c_str());
      disp_atemp_04.setText(String(canMsg_r.data[3]).c_str());
      disp_atemp_05.setText(String(canMsg_r.data[4]).c_str());
      disp_atemp_06.setText(String(canMsg_r.data[5]).c_str());
      disp_atemp_07.setText(String(canMsg_r.data[6]).c_str());
      disp_atemp_08.setText(String(canMsg_r.data[7]).c_str());
    }
  }
  nexLoop(nex_listen_list); //check for button presses on nextion screen
#endif
  smartDelay(0); //loop function for gps module
}