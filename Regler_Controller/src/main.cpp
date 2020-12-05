#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>


#define CAN_ID_CONTROL_LOWER_MOTOR 0xC0
#define CAN_ID_INFOS_LOWER_CONTROLLER 0xC1


#define INFO_SEND_INTERVAL 1000000 //in micro-seconds
#define MAX_TIME_WITHOUT_UPDATE 2000000 //in micro-seconds

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

  //Setting Timer1 to a Fast-PWM-Mode 
  TCCR1A=0b10<<COM1A0|0b10<<WGM10;
  TCCR1B=0b11<<WGM12|0b11<<CS10;
  //TIMSK1=0b110; //Enabeling the overflow interrupts leads to inexplicable problems with the SPI-interface
  
  //Top-Value for a frequency of approximately 60Hz
  ICR1H=0B10000;
  ICR1L=0B0;

  sei(); //Enabeling the interrupts
  
  OCR1AH=0b01;
  OCR1AL=0;

  //initialize canbus-shield
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();

  canMsg_send.can_id=CAN_ID_INFOS_LOWER_CONTROLLER;
  canMsg_send.can_dlc=6;

  //initialize controller-decoder
  jeti->init();
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) { //check for new control data
    if ((canMsg.can_id==CAN_ID_CONTROL_LOWER_MOTOR)&&(canMsg.can_dlc==1)){
        last_data_received=micros();
        char value = canMsg.data[0];
        OCR1AH=0b01;
        OCR1AL=value;
    }
  }

  if (micros()-last_data_received>=MAX_TIME_WITHOUT_UPDATE){//Stop the motor if no data from CanBus was received for 2 seconds
    OCR1AH=0b01;
    OCR1AL=0;
  }


  jeti->loop();
  if ((micros()-last_data_sent)>=INFO_SEND_INTERVAL&&jeti->isNewMsg()){ //send new controller info to the canbus
    jetiTelemetry_t tel = JetiBase::getTelemetry(jeti->getMsg());

    double voltage_100=tel.voltage*100.0;
    int voltage=(int)voltage_100;
    canMsg_send.data[0]=voltage>>8;
    canMsg_send.data[1]=voltage&0xFF;
    canMsg_send.data[2]=tel.temperature&0xFF;
    canMsg_send.data[3]=tel.percent&0xFF;
    canMsg_send.data[4]=tel.rpm>>8;
    canMsg_send.data[5]=tel.rpm&0xFF;
    mcp2515.sendMessage(&canMsg_send);
    last_data_sent=micros();
  }
}