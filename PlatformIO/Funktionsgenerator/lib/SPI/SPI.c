#include "SPI.h"

/* -------------------- PRIVATE DEFINES -------------------- */
#define CH 7
#define GA 5
#define SHDN 4

/* -------------------- PRIVATE PROTOTYPES -------------------- */

unsigned char SPI_transmit(unsigned char send);

/* -------------------- PUBLIC FUNCTIONS -------------------- */

/*
 * Initializiert SPI
 *
 * maximaler Geschwindgkeit
 * ohne Interrupts
 * init ADC
 * init DAC
 */
void SPI_init() {
	// init SPI ports
	DDRB = (DDRB&0b11000011)|(1<<PB2)|(1<<PB3)|(1<<PB5); //PIN 4 auf 0 setzten, 2,3,5 auf 1
	PORTB &= ~(1<<4);

	// init adc/dac ports
	CS_ADC_DDR |= 1<<CS_ADC_PIN;
	CS_ADC_PORT |= 1<<CS_ADC_PIN;
	CS_DAC_DDR |= 1<<CS_DAC_PIN;
	CS_DAC_PORT |= 1<<CS_DAC_PIN;
	LDAC_DAC_DDR |= 1<<LDAC_DAC_PIN;
	LDAC_DAC_PORT |= 1<<LDAC_DAC_PIN;

	// init SPI
	SPCR = (1<<SPE)|(1<<MSTR);
	SPSR = 0;
}

/*
 * Gibt einen Wert (12bit) aus
 */
void SPI_DAC_out(unsigned char channel, unsigned int value){
	unsigned char out [2];
	// Bestimme die bytes
	out[0] = ((channel&1)<<CH) | (1<<SHDN) | ((value>>8)&0xF);
	out[1] = value&0xFF;
	// CS setzten
	CS_DAC_PORT &=~(1<<CS_DAC_PIN);
	// Übertragung
	for(int i = 0; i < 2; i++)
		SPI_transmit(out[i]);
	// CS reset
	CS_DAC_PORT |= (1<<CS_DAC_PIN);
	// LDAC Impuls
	LDAC_DAC_PORT &=~(1<<LDAC_DAC_PIN);
	LDAC_DAC_PORT |=(1<<LDAC_DAC_PIN);
}

/*
 * Gibt einen Wert (12bit) zurück
 */
unsigned int SPI_ADC_in(char channel){
	CS_DAC_PORT &=~(1<<CS_ADC_PIN);
	SPI_transmit(0b1100000);
	unsigned int in1 = (unsigned int) SPI_transmit(0);
	unsigned int in2 = (unsigned int) SPI_transmit(0);
	CS_DAC_PORT |=(1<<CS_ADC_PIN);
	return ((in1<<4) | ((in2>>4)&15));
}

/* -------------------- PRIVATE FUNCTIONS -------------------- */

/*
 * sendet ein Byte
 * gibt das empfange zurück
 */
unsigned char SPI_transmit(unsigned char send){
	SPDR = send;
	// wait for flag
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}
