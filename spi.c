#include "spi.h"

void spi_init() {
	SPI_DDR |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
	//
	DDRB &= ~(1<<SPI_MISO);	
	SPSR &= ~(1<<SPI2X);
	//
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);	//prescaler 16
}


uint8_t spi_transmit(uint8_t data) {
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	
	return SPDR;
}

