#include <JetiModes.hpp>
#if JETI_MODE == ARDUINO_UNO

#ifndef __JETI_UNO_HPP__
#define __JETI_UNO_HPP__

#include <Arduino.h>
#include <inttypes.h>
#include <stdlib.h>
#include "JetiBase.hpp"

JetiBase *jeti_pointer = nullptr;

class Jeti_UNO : public JetiBase
{
public:
    Jeti_UNO() : JetiBase()
    {
        jeti_pointer = this;
    }

    // inits Serial Communication (still need to call sei() in main setup)
    void init()
    {
        // serial 1 -- 9600baud 9bit, odd, 2stop
        UCSR0A = 0;
        UCSR0B = 1 << RXCIE0 | 1 << RXEN0 | 1 << UCSZ02; //RXB8n
        UCSR0C = 3 << UPM00 | 1 << USBS0 | 3 << UCSZ00;
        UBRR0H = 0;
        UBRR0L = 103;
        DDRD &= ~(1 << PD1); // set input
        PORTD |= (1 << PD1); // set pullup
    }

    // interrrupt funcitons
    void interrupt_RX()
    {
        start = UCSR0B & (1 << RXB80);
        msg = UDR0;
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
        UCSR0B &= ~(1 << RXEN0);  // disable recieve
        UCSR0B &= ~(1 << RXCIE0); // disable recieve interrupt
        DDRD |= (1 << PD1);       // set output
        UCSR0B |= 1 << TXEN0;     // enable transmit
        UCSR0B |= (1 << TXCIE0);  // enable transmite complete
        //_delay_ms(1);
        if (UCSR0A & (1 << UDRE0))
        {
            actualSend();
            if (sendb > 1)
                goto enableInterrupt;
        }
        else
        {
        enableInterrupt:
            UCSR0B |= 1 << UDRIE0; // enable send interrrupt
        }
    }
    // actual send function
    void actualSend()
    {
        UCSR0B &= ~(1 << TXB80);
        UDR0 = ((!buttonL) & 1) << 7 | ((!buttonD) & 1) << 6 | ((!buttonU) & 1) << 5 | ((!buttonR) & 1) << 4;
    }

    // reconfigure recieve mode
    void actualsendDone()
    {
        UCSR0B &= ~(1 << UDRIE0); // disable send interrupt
        UCSR0B &= ~(1 << TXCIE0); // disable transmit interrupt
        UCSR0B &= ~(1 << TXEN0);  // disable transmit
        UCSR0A |= (1 << TXC0);
        DDRD &= ~(1 << PD1); // set input
        PORTD |= (1 << PD1); // set pullup
        while (!(PIND & (1 << PD1)))
            ;
        //_delay_ms(1);
        UCSR0B |= (1 << RXEN0);  // enable recieve
        UCSR0B |= (1 << RXCIE0); // disable recieve interrupt
        sendDone = false;
    }
};

typedef Jeti_UNO Jeti;

ISR(USART_RX_vect)
{
    if (jeti_pointer != nullptr)
        jeti_pointer->interrupt_RX();
}

ISR(USART_UDRE_vect)
{
    if (jeti_pointer != nullptr)
        jeti_pointer->interrupt_UDRE();
}

ISR(USART_TX_vect)
{
    if (jeti_pointer != nullptr)
        jeti_pointer->interrupt_TX();
}

#endif /*__Jeti_ATmega_Serial1_HPP__*/

#endif