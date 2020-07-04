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
volatile bool buttonD = true;
volatile bool buttonU = false;

volatile bool recieving = false;
volatile bool sendb = false;
volatile bool sendDone = false;

volatile unsigned long rectime = 0;

void actualSend();
void actualSendDone();
void send();

ISR(USART1_RX_vect)
{
  start = UCSR1B & (1 << RXB81);
  msg = UDR1;
  msgb = true;
  recieving = true;
}

ISR(USART1_UDRE_vect)
{
  actualSend();
}

ISR(USART1_TX_vect)
{
  //actualSendDone();
  sendDone = true;
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

  sei(); // enable interrupt

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
    //Serial << _HEX(msg) << " " << start << endl;
    //rectime = millis();
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
  if (sendb && !recieving)
  {
    send();
    Serial << "send" << endl;
    sendb = false;
  }
  if (millis() - lastTime > 1000)
  {
    sendb = true;
    lastTime = millis();
  }
  if (sendDone)
  {
    actualSendDone();
  }

  // dump registers
  /*if (millis() % 1000 == 0)
  {
    Serial << endl;
    Serial << "UCSR1A " << UCSR1A << " 0b" << _BIN(UCSR1A) << endl;
    Serial << "UCSR1B " << UCSR1B << " 0b" << _BIN(UCSR1B) << endl;
    Serial << "UCSR1C " << UCSR1C << " 0b" << _BIN(UCSR1C) << endl;
    Serial << "pin " << ((PIND & (1 << PD3)) ? 1 : 0) << endl;
  }*/
}

void send()
{
  UCSR1B &= ~(1 << RXEN1);  // disable recieve
  UCSR1B &= ~(1 << RXCIE1); // disable recieve interrupt
  DDRD |= (1 << PD3);       // set output
  UCSR1B |= 1 << TXEN1;     //enable transmit
  UCSR1B |= (1 << TXCIE1);  // enable transmite complete
  _delay_ms(2);
  if (UCSR1A & (1 << UDRE1))
  {
    actualSend();
  }
  else
  {
    UCSR1B |= 1 << UDRIE1; // enable send interrrupt
  }
}

void actualSend()
{
  UCSR1B &= ~(1 << TXB81);
  UDR1 = ((!buttonL) & 1) << 7 | ((!buttonD) & 1) << 6 | ((!buttonU) & 1) << 5 | ((!buttonR) & 1) << 4;
}

void actualSendDone()
{
  UCSR1B &= ~(1 << UDRIE1); // disable send interrupt
  UCSR1B &= ~(1 << TXCIE1); // disable transmit interrupt
  UCSR1B &= ~(1 << TXEN1);  // disable transmit
  UCSR1A |= (1<<TXC1);
  DDRD &= ~(1 << PD3); // set input
  PORTD |= (1 << PD3); // set pullup
  while(!(PIND&(1<<PD3)));
  _delay_ms(1);
  UCSR1B |= (1 << RXEN1);  // enable recieve
  UCSR1B |= (1 << RXCIE1); // disable recieve interrupt
}