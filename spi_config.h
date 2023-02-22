/*
 * spi_config.h
 *
 * Created: 1/20/2021 4:02:22 PM
 * Author : Mato
 */ 

#ifndef SPI_CONFIG_H
#define SPI_CONFIG_H

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

#endif
