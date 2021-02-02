#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_ADS1015.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Adafruit_ADS1115 adcleft = Adafruit_ADS1115(0x48);
Adafruit_ADS1115 adcright = Adafruit_ADS1115(0x49);

#include <mcp2515.h>

#define CAN_ID_CONTROL_MOTORS_SERVOS 0xC0
#define CAN_ID_BATTERY_TEMPS_LEFT 0xE0
#define CAN_ID_BATTERY_TEMPS_RIGHT 0xE1
#define SERVO_FREQ 60

/*
Servo 1:
Unterer Grenzwert: 163
Mitte: 322
Obere Grenze: 446
*/

/*
Servo 2 (Mitte)
Unten: 223
Mitte: 355
Oben: 490
*/

/*
Servo 3
Unten: 231
Mitte: 355
Oben: 493
*/

struct can_frame canMsg;
struct can_frame canadcleft;
struct can_frame canadcright;
MCP2515 mcp2515(7);

unsigned long lastsentadc=0;

void setup() {
  Serial.begin(115200);
  Serial.println("8 channel Servo test!");

  pwm.begin();

  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);
  pwm.setPWM(0,0,322);
  pwm.setPWM(4,0,355);
  pwm.setPWM(8,0,355);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();

  adcleft.begin();
  adcright.begin();

  canadcleft.can_dlc=8;
  canadcleft.can_id=CAN_ID_BATTERY_TEMPS_LEFT;
  canadcright.can_dlc=8;
  canadcright.can_id=CAN_ID_BATTERY_TEMPS_RIGHT;
  //canMsg.can_id=CAN_ID_CONTROL_MOTORS_SERVOS;
}

void loop() {

   if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
     if ((canMsg.can_id==CAN_ID_CONTROL_MOTORS_SERVOS)&&(canMsg.can_dlc==3)){
       int anw = map(canMsg.data[2],0,255,-127,127);
        pwm.setPWM(0,0,322+anw);
        pwm.setPWM(4,0,355+anw);
        pwm.setPWM(8,0,355+anw);
     }
   }
   if (millis()-lastsentadc>2000){
    int16_t battemps_left[4];
    int16_t battemps_right[4];
    for (int i=0;i<=3;i++){
      int16_t c_adc_left,c_adc_right;
      c_adc_left=adcleft.readADC_SingleEnded(i);
      c_adc_right=adcright.readADC_SingleEnded(i);
      canadcleft.data[2*i]=(c_adc_left>>8)&0xFF;
      canadcleft.data[1+2*i]=c_adc_left&0xFF;

      canadcright.data[2*i]=(c_adc_right>>8)&0xFF;
      canadcright.data[1+2*i]=c_adc_right&0xFF;
    }
    mcp2515.sendMessage(&canadcleft);
    mcp2515.sendMessage(&canadcright);
    lastsentadc=millis();
    
   }
}

