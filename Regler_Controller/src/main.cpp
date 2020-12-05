#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

#define REGLER_CAN_ID 0xF2

#include <JetiModes.hpp>
#define JETI_MODE ARDUINO_UNO
#include <Jeti.hpp>


JetiBase *jeti = new Jeti_UNO(); // access object via pointer (this allows switching between JETI_MODES with minimal code cahnges)
struct can_frame canMsg;
struct can_frame canMsg_send;
MCP2515 mcp2515(7);

unsigned long last_update=0;

void setup() {


  // //Serial.begin(115200);
  DDRB=DDRB|0B010;
  PORTB=PORTB|0b110;
  TCCR1A=0b10<<COM1A0|0b10<<WGM10;
  TCCR1B=0b11<<WGM12|0b11<<CS10;
  //TIMSK1=0b110;
  ICR1H=0B10000;
  ICR1L=0B0;

  sei();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();

    canMsg_send.can_id=0xF5;
    canMsg_send.can_dlc=3;
    canMsg_send.data[0]=9;
    mcp2515.sendMessage(&canMsg_send);
  jeti->init();
  OCR1AH=0b01;
  OCR1AL=0;


  

  
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if ((canMsg.can_id==REGLER_CAN_ID)&&(canMsg.can_dlc==2)&&(canMsg.data[0]==0x00)){
        char value = canMsg.data[1];
        OCR1AH=0b01;
        OCR1AL=value;
    }
  }
  jeti->loop();
  if ((micros()-last_update)>=1000000&&jeti->isNewMsg()){
    jetiTelemetry_t tel = JetiBase::getTelemetry(jeti->getMsg());
    canMsg_send.can_id=0xF5;
    canMsg_send.can_dlc=3;
    double voltage_100=25.43*100.0;
    int voltage=(int)voltage_100;
    canMsg_send.data[0]=voltage>>8;
    canMsg_send.data[1]=voltage&0xFF;
    canMsg_send.data[2]=tel.temperature;
    mcp2515.sendMessage(&canMsg_send);
    last_update=micros();
    delay(10);
  }
}