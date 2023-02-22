/*
 * usart.c
 *
 * Created: 1/26/2021 10:46:18 PM
 *  Author: Mateo
 */ 

#include "usart.h"
#define F_CPU 7372800UL

void usart_init(uint16_t baudRate) {
	if (baudRate != 2400 && baudRate != 4800 && baudRate != 9600) return;
	// calculate UBRR from baudRate
	uint16_t ubrr = ((F_CPU)/(16UL*baudRate)) - 1;
	UBRRH = (ubrr << 8);
	UBRRL = ubrr;
	
	// enable receive and transmit
	UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
	
	// frame format: 8 data bits, 1 stop bit, no parity
	UCSRC = _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1) ;
}

void usart_putc(const unsigned char data) {
	while ( !( UCSRA & _BV(UDRE)) );
	UDR = data;
	uint16_t i;
	if (i == 10){
		i = 0;
	}
	lcd_putc(data);
}

void usart_puts(const char* data) {
	while(*data != 0x00){
		usart_putc(*data);
		data++;
	}
}

void usart_hex(uint8_t d) {
	char byte = '0';
	// send lower nibble as hex char
	(((d>>4)&0x0F)<=9) ? (byte='0'+((d>>4)&0x0F)) : (byte='A'+ ((d>>4)&0x0F)-0x0A);
	usart_putc(byte);
	// send higher nibble as hex char
	((d&0x0F)<=9) ? (byte='0'+ (d&0x0F)) : (byte='A'+ (d&0x0F)-0x0A);
	usart_putc(byte);
}
