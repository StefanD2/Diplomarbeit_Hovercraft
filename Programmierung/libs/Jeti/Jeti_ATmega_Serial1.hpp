/*
* Jeti-Library by Stefan Deimel, 23.10.2020
* Jeti_ATmega_Serial1.hpp -- platform dependent functions for Arduino Mega using Serial 1
*/
#include <JetiModes.hpp>
#if JETI_MODE == ARDUINO_MEGA_1

#ifndef __Jeti_ATmega_Serial1_HPP__
#define __Jeti_ATmega_Serial1_HPP__

#include <Arduino.h>
#include <inttypes.h>
#include <stdlib.h>
#include "JetiBase.hpp"

// pointer to jeti-base object, to preventent multiple objects Jeti_UNO from causing problems
JetiBase *jeti_pointer = nullptr;

class Jeti_ATmega_Serial1 : public JetiBase
{
public:
    Jeti_ATmega_Serial1() : JetiBase()
    {
        jeti_pointer = this;
    }

    // initialises Serial Communication (still need to call sei() in main setup)
    void init()
    {
        // initialise serial communication -- 9600baud, 9bit, odd parity, 2 stop bits
        UCSR1A = 0;
        UCSR1B = 1 << RXCIE1 | 1 << RXEN1 | 1 << UCSZ12;
        UCSR1C = 3 << UPM10 | 1 << USBS1 | 3 << UCSZ10;
        UBRR1H = 0;
        UBRR1L = 103;
        DDRD &= ~(1 << PD3); // set input
        PORTD |= (1 << PD3); // set pullup
    }

    // overwrite interrrupt functions
    void interrupt_RX()
    {
        recieving = true;
        recieve_buffer[recieve_buffer_write].bit9 = UCSR1B & (1 << RXB81); // 9th bit, must be read first
        recieve_buffer[recieve_buffer_write].databyte = UDR1;
        recieve_buffer_write++;
        recieve_buffer_write &= (RECIEVE_BUFFER_SIZE - 1);
    }

    void interrupt_TX()
    {
        send_count--;
        sendDone = (send_count > 0) ? 0 : 1;
    }

    void interrupt_UDRE()
    {
        send();
    }

private:
    // function to initialise send
    void send_init()
    {
        UCSR1B &= ~(1 << RXEN1);  // disable recieve
        UCSR1B &= ~(1 << RXCIE1); // disable recieve interrupt
        DDRD |= (1 << PD3);       // set output
        UCSR1B |= 1 << TXEN1;     //enable transmit
        UCSR1B |= (1 << TXCIE1);  // enable transmite complete
        //_delay_ms(1);
        if (UCSR1A & (1 << UDRE1))
        {
            send();
            if (send_count > 1)
                goto enableInterrupt;
        }
        else
        {
        enableInterrupt:
            UCSR1B |= 1 << UDRIE1; // enable send interrrupt
        }
    }
    
    // actual send function
    void send()
    {
        UCSR1B &= ~(1 << TXB81);
        UDR1 = ((!buttonL) & 1) << 7 | ((!buttonD) & 1) << 6 | ((!buttonU) & 1) << 5 | ((!buttonR) & 1) << 4;
    }

    // reenable recieve mode
    void send_done()
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

// interrupt vectors

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