/*
 * usart.h
 *
 * Created: 1/26/2021 10:45:56 PM
 *  Author: Mateo
 */ 


#ifndef USART_H_
#define USART_H_
#include <avr/io.h>



/*
* Initializes USART with a given baud rate
* Valid baud rates are: 2400, 4800, 9600
*/
void usart_init(uint16_t baudRate);

/*
* Sends a character
*/
void usart_putc(const unsigned char data);

/*
* Sends a string of data ending with a terminating character '\0'
*/
void usart_puts(const char* data); 

/*
* Sends data as hex characters
*/
void usart_hex(uint8_t d);


#endif /* USART_H_ */