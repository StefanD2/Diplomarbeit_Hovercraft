/*******************************************************************************
 *
 * Autor: Stefan Deimel
 *
 * Datum: 15.10.2019
 *
 *******************************************************************************/

#ifndef __UART_PROTOKOLL__
#define __UART_PROTOKOLL__

#include <stdlib.h>
#include "UART_Ringpuffer.h"

// ------------------------------ CONFIGURATION ------------------------------

// starting bytes
#define STARTBYTE ""
// length of starting bytes
#define STARTBYTE_LENGTH 0

// stopping bytes
#define STOPPBYTE ""
// length of stopping bytes
#define STOPPBYTE_LENGTH 0

// length of data
#define DATA_LENGTH 1

// how often a frame should fit in the buffer
#define FRAMES_PER_PUFFER 2

// additional bytes after stop bytes which are not part of the frame
#define ADDITIONAL_ENDBYTE ""
// length of additional bytes
#define ADDITIONAL_ENDBYTE_LENGTH 0

// ------------------------------ PUBLIC PROTOTYPES ------------------------------

// --------------- SEND ---------------
// initialize UART_Protkoll and buffer to send (send and receive can be initialized at the same time)
void UART_PROTOKOLL_init_send();
// send a value
// returns 1 if success, 0 if failure
char UART_PROTOKOLL_send(int val);

// --------------- RECEIVE ---------------
// initialize UART_Protkoll and buffer to receive
// newData is the number how much unread frames are received
void UART_PROTOKOLL_init_receive(volatile char * newData);
// get the next unread value
// returns -1 if no frame was found
int UART_PROTOKOLL_receive();

#endif /* __UART_PROTOKOLL__ */
