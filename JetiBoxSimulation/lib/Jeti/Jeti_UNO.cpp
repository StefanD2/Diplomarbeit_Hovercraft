#include "Jeti_UNO.hpp"

#include <avr/interrupt.h>
#include <avr/io.h>

Jeti_UNO *jeti_uno_pointer = nullptr;

ISR(USART_RX_vect)
{
    if (jeti_uno_pointer != nullptr)
        jeti_uno_pointer->interrupt_RX();
}

ISR(USART_UDRE_vect)
{
    if (jeti_uno_pointer != nullptr)
        jeti_uno_pointer->interrupt_UDRE();
}

ISR(USART_TX_vect)
{
    if (jeti_uno_pointer != nullptr)
        jeti_uno_pointer->interrupt_TX();
}

Jeti_UNO::Jeti_UNO() : Jeti()
{
    jeti_uno_pointer = this;
}

void Jeti_UNO::init()
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

void Jeti_UNO::csend()
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

void Jeti_UNO::actualSend()
{
    UCSR0B &= ~(1 << TXB80);
    UDR0 = ((!buttonL) & 1) << 7 | ((!buttonD) & 1) << 6 | ((!buttonU) & 1) << 5 | ((!buttonR) & 1) << 4;
}

void Jeti_UNO::actualsendDone()
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

void Jeti_UNO::interrupt_RX()
{
    start = UCSR0B & (1 << RXB80);
    msg = UDR0;
    msgb = true;
    recieving = true;
}

void Jeti_UNO::interrupt_TX()
{
    sendb--;
    sendDone = (sendb>0)?0:1;
}

void Jeti_UNO::interrupt_UDRE()
{
    actualSend();
}