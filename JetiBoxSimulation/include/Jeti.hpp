#ifndef __JETI_HPP__
#define __JETI_HPP__

#include <Arduino.h>

class Jeti
{
public:
    Jeti();
    virtual void init() = 0;                   // inits Serial Communication (still need to call sei() in main setup)
    void loop();                               // loop function
    void send(bool l, bool r, bool u, bool d); // sends buttons
    void send(bool l, bool r, bool u, bool d, int count); // sends buttons multiple times
    bool isNewMsg();                           // checks if new msg has been recieved
    String getMsg();                           // get msg

    // interrrupt funcitons
    virtual void interrupt_RX() = 0;
    virtual void interrupt_TX() = 0;
    virtual void interrupt_UDRE() = 0;

protected:
    virtual void csend() = 0;           // send function
    virtual void actualSend() = 0;     // actual send function
    virtual void actualsendDone() = 0; // reconfigure recieve mode

    volatile char msg;
    volatile bool start;
    volatile bool msgb;
    volatile bool recieving;
    volatile bool sendDone;
    volatile bool buttonL;
    volatile bool buttonR;
    volatile bool buttonD;
    volatile bool buttonU;
    volatile int sendb;
    String content;
    String newMsgString;
    bool newMsg;
    bool adding;
};

#endif /*__JETI_HPP__*/