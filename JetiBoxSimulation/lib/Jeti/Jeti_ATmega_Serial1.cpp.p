#include "Jeti_ATmega_Serial1.hpp"

#include <avr/interrupt.h>
#include <avr/io.h>

Jeti_ATmega_Serial1 *jeti_atmega_serial1_pointer = nullptr;

ISR(USART1_RX_vect)
{
    if (jeti_atmega_serial1_pointer != nullptr)
        jeti_atmega_serial1_pointer->interrupt_RX();
}

ISR(USART1_UDRE_vect)
{
    if (jeti_atmega_serial1_pointer != nullptr)
        jeti_atmega_serial1_pointer->interrupt_UDRE();
}

ISR(USART1_TX_vect)
{
    if (jeti_atmega_serial1_pointer != nullptr)
        jeti_atmega_serial1_pointer->interrupt_TX();
}

Jeti_ATmega_Serial1::Jeti_ATmega_Serial1() : Jeti()
{
    jeti_atmega_serial1_pointer = this;
}

void Jeti_ATmega_Serial1::init()
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

void Jeti_ATmega_Serial1::csend()
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

void Jeti_ATmega_Serial1::actualSend()
{
    UCSR1B &= ~(1 << TXB81);
    UDR1 = ((!buttonL) & 1) << 7 | ((!buttonD) & 1) << 6 | ((!buttonU) & 1) << 5 | ((!buttonR) & 1) << 4;
}

void Jeti_ATmega_Serial1::actualsendDone()
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

void Jeti_ATmega_Serial1::interrupt_RX()
{
    start = UCSR1B & (1 << RXB81);
    msg = UDR1;
    msgb = true;
    recieving = true;
}

void Jeti_ATmega_Serial1::interrupt_TX()
{
    sendb--;
    sendDone = (sendb>0)?0:1;
}

void Jeti_ATmega_Serial1::interrupt_UDRE()
{
    actualSend();
}