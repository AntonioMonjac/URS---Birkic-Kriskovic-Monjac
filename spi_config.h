#ifndef SPI_CONFIG_H
#define SPI_CONFIG_H

#include <avr/io.h>

// SPI ports
#define SPI_DDR		DDRB
#define SPI_PORT	PORTB
#define SPI_PIN		PINB
#define SPI_MOSI	5
#define SPI_MISO	6
#define SPI_SS		4
#define SPI_SCK		7

#endif
