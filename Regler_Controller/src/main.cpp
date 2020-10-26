#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

#define REGLER_CAN_ID 0xF7

struct can_frame canMsg;
MCP2515 mcp2515(7);


void setup() {
  Serial.begin(115200);
  DDRB=DDRB|0B10;
  TCCR1A=0b10<<COM1A0|0b10<<WGM10;
  TCCR1B=0b11<<WGM12|0b11<<CS10;
  //TIMSK1=0b10|1<<ICIE1;

  ICR1H=0B10000;
  ICR1L=0B0;
  
  sei();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();

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
}