#include <JetiModes.hpp>
#if JETI_MODE == ARDUINO_MEGA_1

#ifndef __Jeti_ATmega_Serial1_HPP__
#define __Jeti_ATmega_Serial1_HPP__

#include <Arduino.h>
#include <inttypes.h>
#include <stdlib.h>
#include "JetiBase.hpp"

JetiBase *jeti_pointer = nullptr;

class Jeti_ATmega_Serial1 : public JetiBase
{
public:
    Jeti_ATmega_Serial1() : JetiBase()
    {
        jeti_atmega_serial1_pointer = this;
    }

    // inits Serial Communication (still need to call sei() in main setup)
    void init()
    {
        // serial 1 -- 9600baud 9bit, odd, 2stop
        UCSR1A = 0;
        UCSR1B = 1 << RXCIE1 | 1 << RXEN1 | 1 << UCSZ12; //RXB8n
        UCSR1C = 3 << UPM10 | 1 << USBS1 | 3 << UCSZ10;
        UBRR1H = 0;
        UBRR1L = 103;
        DDRD &= ~(1 << PD3); // set input
        PORTD |= (1 << PD3); // set pullup
    }

    // interrrupt funcitons
    void interrupt_RX()
    {
        start = UCSR1B & (1 << RXB81);
        msg = UDR1;
        msgb = true;
        recieving = true;
    }

    void interrupt_TX()
    {
        sendb--;
        sendDone = (sendb > 0) ? 0 : 1;
    }

    void interrupt_UDRE()
    {
        actualSend();
    }

private:
    // send function
    void csend()
    {
        UCSR1B &= ~(1 << RXEN1);  // disable recieve
        UCSR1B &= ~(1 << RXCIE1); // disable recieve interrupt
        DDRD |= (1 << PD3);       // set output
        UCSR1B |= 1 << TXEN1;     //enable transmit
        UCSR1B |= (1 << TXCIE1);  // enable transmite complete
        //_delay_ms(1);
        if (UCSR1A & (1 << UDRE1))
        {
            actualSend();
            if (sendb > 1)
                goto enableInterrupt;
        }
        else
        {
        enableInterrupt:
            UCSR1B |= 1 << UDRIE1; // enable send interrrupt
        }
    }
    // actual send function
    void actualSend()
    {
        UCSR1B &= ~(1 << TXB81);
        UDR1 = ((!buttonL) & 1) << 7 | ((!buttonD) & 1) << 6 | ((!buttonU) & 1) << 5 | ((!buttonR) & 1) << 4;
    }

    // reconfigure recieve mode
    void actualsendDone()
    {
        UCSR1B &= ~(1 << UDRIE1); // disable send interrupt
        UCSR1B &= ~(1 << TXCIE1); // disable transmit interrupt
        UCSR1B &= ~(1 << TXEN1);  // disable transmit
        UCSR1A |= (1 << TXC1);
        DDRD &= ~(1 << PD3); // set input
        PORTD |= (1 << PD3); // set pullup
        while (!(PIND & (1 << PD3)))
            ;
        //_delay_ms(1);
        UCSR1B |= (1 << RXEN1);  // enable recieve
        UCSR1B |= (1 << RXCIE1); // disable recieve interrupt
        sendDone = false;
    }
};

typedef Jeti_ATmega_Serial1 Jeti;

ISR(USART1_RX_vect)
{
    if (jeti_pointer != nullptr)
        jeti_pointer->interrupt_RX();
}

ISR(USART1_UDRE_vect)
{
    if (jeti_pointer != nullptr)
        jeti_pointer->interrupt_UDRE();
}

ISR(USART1_TX_vect)
{
    if (jeti_pointer != nullptr)
        jeti_pointer->interrupt_TX();
}

#endif /*__Jeti_ATmega_Serial1_HPP__*/

#endif