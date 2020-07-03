/*******************************************************************************
 *
 * Autor: Stefan Deimel
 *
 * Datum: 15.10.2019
 *
 *******************************************************************************/

#ifndef __UART_RINGPUFFER__
#define __UART_RINGPUFFER__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <math.h>

// Baud rate
#define BAUD 921600

// initialize buffer and UART to SEND
// size: size of buffer as exponent of 2
void UART_RINGPUFFER_init_send(int size);

// initialize buffer and UART to RECIEVE
// size: size of buffer as exponent of 2
void UART_RINGPUFFER_init_receive(int size,
		void (*function)(volatile unsigned char *, size_t,
				volatile size_t, volatile size_t));

// sends data if buffer has enough free space
// returns 1 if success, if not enough space returns 0
char UART_RINGPUFFER_send(unsigned char* data, size_t length);

// returns unread bytes from buffer
unsigned char * UART_RINGPUFFER_receive();
// returns size of the unread buffer
size_t UART_RINGPUFFER_receive_size();

#endif /* __UART_RINGPUFFER__ */
