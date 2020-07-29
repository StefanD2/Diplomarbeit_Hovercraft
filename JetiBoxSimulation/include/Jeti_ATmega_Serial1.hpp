#ifndef __Jeti_ATmega_Serial1_HPP__
#define __Jeti_ATmega_Serial1_HPP__

#include <Arduino.h>
#include <inttypes.h>
#include <stdlib.h>
#include "Jeti.hpp"

class Jeti_ATmega_Serial1 : public Jeti
{
public:
    Jeti_ATmega_Serial1();
    void init(); // inits Serial Communication (still need to call sei() in main setup)

    // interrrupt funcitons
    void interrupt_RX();
    void interrupt_TX();
    void interrupt_UDRE();

private:
    void csend();           // send function
    void actualSend();     // actual send function
    void actualsendDone(); // reconfigure recieve mode
};

#endif /*__Jeti_ATmega_Serial1_HPP__*/