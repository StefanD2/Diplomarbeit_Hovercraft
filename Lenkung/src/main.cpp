#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#include <mcp2515.h>

#define CAN_ID_CONTROL_MOTORS_SERVOS 0xC0
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
MCP2515 mcp2515(7);

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
}

