#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

//----------- Do not change these values! ----------------------
#define CAN_ID_CONTROL_MOTORS_SERVOS 0xC0
#define CAN_ID_INFOS_LOWER_CONTROLLER 0xC1
#define CAN_ID_INFOS_BACK_CONTROLLER 0xC3
//--------------------------------------------------------------
#define MIN_PWM_VALUE 30

#define CONTROLLER_ID 1 //either 0 for bottom or 1 for back controller

#define INFO_SEND_INTERVAL 1000000 //in micro-seconds
#define MAX_TIME_WITHOUT_UPDATE 4000000 //in micro-seconds



#if not (CONTROLLER_ID==0 or CONTROLLER_ID==1)
#error wrong controller ID defined!
#endif

#include <JetiModes.hpp>
#define JETI_MODE ARDUINO_UNO
#include <Jeti.hpp>


JetiBase *jeti = new Jeti_UNO();
MCP2515 mcp2515(7);

//CAN-Bus-Data-Frames
struct can_frame canMsg;
struct can_frame canMsg_send;

unsigned long last_data_sent=0;
unsigned long last_data_received=0;

void setup() {

  DDRB=DDRB|0B010; //Setting Port D9 as an output
  PORTB=PORTB|0b110;

  //initialize Timer1 to a Fast-PWM-Mode 
  TCCR1A=0b10<<COM1A0|0b10<<WGM10; //Clear OC1A/OC1B on Compare Match + Fast PWM (TOP = ICR1)
  TCCR1B=0b11<<WGM12|0b11<<CS10; //Prescaler 64 + Fast PWM
  //TIMSK1=0b110; //Enabeling the overflow interrupts leads to inexplicable problems with the SPI-interface
  
  //Top-value of Timer1 for a frequency of approximately 60Hz
  ICR1H=0B10000;
  ICR1L=0B0;

  sei(); //Enabeling the interrupts

  OCR1AH=0b01;
  OCR1AL=MIN_PWM_VALUE;

  //initialize canbus-shield
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();
  #if CONTROLLER_ID == 0
  canMsg_send.can_id=CAN_ID_INFOS_LOWER_CONTROLLER;
  #endif
  #if CONTROLLER_ID == 1
  canMsg_send.can_id=CAN_ID_INFOS_BACK_CONTROLLER;
  #endif
  canMsg_send.can_dlc=6; //set canbus-package-length

  jeti->init(); //initialize controller-decoder
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) { //check for new control data
    if ((canMsg.can_id==CAN_ID_CONTROL_MOTORS_SERVOS)&&(canMsg.can_dlc==3)){ //check if correct message was received and if its length is correct
        last_data_received=micros(); //store timestamp
          #if CONTROLLER_ID == 0
            int value = canMsg.data[0];
          #endif
          #if CONTROLLER_ID == 1
            int value = canMsg.data[1];
          #endif
        
        OCR1AH=0b01;
        OCR1AL=max(value,MIN_PWM_VALUE); //set PWM duty cycle according to received value 
    }
  }

  if (micros()-last_data_received>=MAX_TIME_WITHOUT_UPDATE){//stop the motor if no data from CAN-Bus was received for 400 milliseconds
    OCR1AH=0b01;
    OCR1AL=MIN_PWM_VALUE;
  }


  jeti->loop();
  if ((micros()-last_data_sent)>=INFO_SEND_INTERVAL&&jeti->isNewMsg()){
    jetiTelemetry_t tel = JetiBase::getTelemetry(jeti->getMsg()); //decode latest message from controller

    double voltage_100=tel.voltage*100.0; //multiply voltage by 100 to avoid floating point numbers
    int voltage=(int)voltage_100; //typecasting voltage*100.0 to integer
    canMsg_send.data[0]=voltage>>8; //Voltage High-Byte
    canMsg_send.data[1]=voltage&0xFF; //Voltage Low-Byte
    canMsg_send.data[2]=tel.temperature&0xFF; //Temperature
    canMsg_send.data[3]=tel.percent&0xFF; //Power [%]
    canMsg_send.data[4]=tel.rpm>>8; // Rotations High-Byte
    canMsg_send.data[5]=tel.rpm&0xFF; //Rotations Low-Byte
    mcp2515.sendMessage(&canMsg_send); //send new controller info to the canbus
    last_data_sent=micros(); //store timestamp of sending controller telemetry
  }
}