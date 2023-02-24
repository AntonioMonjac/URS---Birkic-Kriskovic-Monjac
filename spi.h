#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <avr/io.h>

/*
*  Note - mfrc522 operates only in slave mode
 */

// SPI ports
#define SPI_DDR		DDRB
#define SPI_PORT	PORTB
#define SPI_PIN		PINB
#define SPI_MOSI	5
#define SPI_MISO	6
#define SPI_SS		4
#define SPI_SCK		7


// Initializes spi
void spi_init();

uint8_t spi_transmit(uint8_t data);

#define SS_ENABLE() (SPI_PORT &= (~(1<<SPI_SS)))

#define SS_DISABLE() (SPI_PORT |= (1<<SPI_SS))


#endif
