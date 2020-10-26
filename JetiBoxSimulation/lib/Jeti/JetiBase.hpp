#ifndef __JetiBase_HPP__
#define __JetiBase_HPP__

#include <Arduino.h>

#define RECIEVE_BUFFER_SIZE 16

// struct for data/telemtrie from jetibox
typedef struct
{
    int percent;
    long rpm;
    double voltage;
    int temperature;
} jetiTelemetry_t;


// struct for 9 bit Serial
typedef struct
{
    char databyte; // data byte (first 8 bits)
    bool bit9; // 9. bit of data
} Serial9bit_t;

class JetiBase
{
public:
    JetiBase();              // Constructor
    virtual void init() = 0; // inits Serial Communication (still need to call sei() in main setup)
    void loop();             // loop function

    void send(bool l, bool r, bool u, bool d);            // sends buttons
    void send(bool l, bool r, bool u, bool d, int count); // sends buttons multiple times

    bool isNewMsg(); // checks if new msg has been recieved
    String getMsg(); // get msg

    // convert msg (String)
    static jetiTelemetry_t getTelemetry(String msg);

    // interrrupt functions (internall functions, must not be called in main)
    virtual void interrupt_RX() = 0;
    virtual void interrupt_TX() = 0;
    virtual void interrupt_UDRE() = 0;

protected:
    virtual void send_init() = 0; // initalize send and start sending first msg, enables send interrupts if send_count is higher then one
    virtual void send() = 0;      // send msg (gets called in interrupt and send_init())
    virtual void send_done() = 0; // disable send interrupts and reenable recieve mode

    volatile Serial9bit_t recieve_buffer[RECIEVE_BUFFER_SIZE]; // recieve buffer
    volatile size_t recieve_buffer_write;                      // points where to write into recieve buffer
    volatile size_t recieve_buffer_read;                       // points where to read from recieve buffer

    volatile bool recieving; // currently recieving message (start has been send, but message is not finished yet, important to avoid sending while recieving)
    volatile bool sendDone;  // wether sending has finished

    // buttons status to send (gets send in send function)
    volatile bool buttonL;   // left button
    volatile bool buttonR;   // right button
    volatile bool buttonD;   // down button
    volatile bool buttonU;   // up button
    volatile int send_count; // how often message has to bee send

    String recieving_msg; // storage for messages which are currently recieved (second recieve buffer)

    String newMsgString; // fully recieved message, gets overrwritten by next completed msg
};

#endif /*__JetiBase_HPP__*/