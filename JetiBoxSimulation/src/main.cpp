#include <stdlib.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include <Arduino.h>
#include <Streaming.h>

volatile bool start = false;
volatile bool msgb = false;
volatile unsigned char msg = 0;

volatile bool buttonL = false;
volatile bool buttonR = false;
volatile bool buttonD = false;
volatile bool buttonU = false;

volatile bool recieving = false;
volatile bool send = false;

volatile unsigned long rectime = 0;

ISR(USART1_RX_vect)
{
  start = UCSR1B & (1 << RXB81);
  msg = UDR1;
  msgb = true;
  recieving = true;
}

ISR(USART1_UDRE_vect)
{
  if (!recieving)
  {
    if (send)
    {
      // 8. bit is zero
      UCSR1B &= ~(1 << TXB81);
      UDR1 = ((!buttonL) & 1) << 7 | ((!buttonD) & 1) << 6 | ((!buttonU) & 1) << 5 | ((!buttonR) & 1) << 4;
      send = false;
    }
    /*else
    {
      UCSR1B &= ~(1 << UDRIE1); // disable send interrupt
      UCSR1B &= ~(1 << TXEN1);
      UCSR1B |= (1 << RXEN1); // enable recieve
      DDRD &= ~(1 << PD3);    // set input
      PORTD |= (1 << PD3);    // set pullup
    }*/
  }
}

ISR(USART1_TX_vect)
{
  UCSR1B &= ~(1 << TXCIE1);
  UCSR1B &= ~(1 << UDRIE1); // disable send interrupt
  UCSR1B &= ~(1 << TXEN1);
  UCSR1B |= (1 << RXEN1); // enable recieve
  DDRD &= ~(1 << PD3);    // set input
  PORTD |= (1 << PD3);    // set pullup
}

void setup()
{
  // serial 1 -- 9600baud 9bit, odd, 2stop

  // 9th in UCSRnB TXB8bit befor UDRn low byte
  // 9th read first
  UCSR1A = 0;
  UCSR1B = 1 << RXCIE1 | 1 << RXEN1 | 1 << UCSZ12; //RXB8n
  UCSR1C = 3 << UPM10 | 1 << USBS1 | 3 << UCSZ10;
  UBRR1H = 0;
  UBRR1L = 103;
  DDRD &= ~(1 << PD3); // set input
  PORTD |= (1 << PD3); // set pullup

  sei();

  Serial.begin(921600);
  Serial << "START" << endl;
}
// 8bit nutzdaten, 9tes bit für start stop --> got it usefull

String string = "";

bool adding = false;

unsigned long lastTime = 0;

void loop()
{
  if (msgb)
  {
    rectime = millis();
    if (!start)
    {
      if (adding)
      {
        Serial << string << endl;
        recieving = false;
      }
      string = "";
      adding = false;
    }
    else
    {
      string += (char)msg;
      adding = true;
    }
    msgb = false;
  }
  if (millis() - lastTime > 1000 && !recieving)
  {
    _delay_ms(1);
    buttonD = true;
    UCSR1B &= ~(1 << RXEN1); // disable recieve
    UCSR1B |= 1 << TXEN1;
    DDRD |= (1 << PD3);      // set output
    UCSR1B |= (1 << TXCIE1); // enable transmite complete
    if (UCSR1A & (1 << UDRE1))
    {
      Serial << "button 0b" << _BIN(((!buttonL) & 1) << 7 | ((!buttonD) & 1) << 6 | ((!buttonU) & 1) << 5 | ((!buttonR) & 1) << 4) << endl;
      UCSR1B &= ~(1 << TXB81);
      UDR1 = ((!buttonL) & 1) << 7 | ((!buttonD) & 1) << 6 | ((!buttonU) & 1) << 5 | ((!buttonR) & 1) << 4;
    }
    else
    {
      send = true;
      UCSR1B |= 1 << UDRIE1; // enable send interrrupt
    }
    lastTime = millis();
  }
  /*if (millis() % 1000 == 0)
  {
    Serial << "UCSR1A " << UCSR1A << endl;
    Serial << "UCSR1B " << UCSR1B << endl;
    Serial << "UCSR1C " << UCSR1C << endl;
  }*/
}
