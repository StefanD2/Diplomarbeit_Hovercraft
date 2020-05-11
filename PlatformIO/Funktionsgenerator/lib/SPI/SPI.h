/*******************************************************************************
 *
 * Autor: Stefan Deimel
 *
 * Datum: 26.11.2019
 *
 *******************************************************************************/

#ifndef __LIB_SPI_H__
#define __LIB_SPI_H__

#include <avr/io.h>

#define SPI_OUT_A 0
#define SPI_OUT_B 1

//---PINS---
//ADC
#define CS_ADC_PORT PORTB
#define CS_ADC_DDR DDRB
#define CS_ADC_PIN 0

//DAC
#define CS_DAC_PORT PORTB
#define CS_DAC_DDR DDRB
#define CS_DAC_PIN 1

#define LDAC_DAC_PORT PORTD
#define LDAC_DAC_DDR DDRD
#define LDAC_DAC_PIN 4

// Initializiert SPI mit maximaler Geschwindgkeit ohne Interrupts
void SPI_init();

// setzt den DAC
void SPI_DAC_out(unsigned char channel, unsigned int value);

//liest den ADC
unsigned int SPI_ADC_in(char channel);

#endif /* __LIB_SPI_H__ */
