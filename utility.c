/*
 * utility.c
 *
 * Created: 2/18/2021 11:24:42 PM
 *  Author: Mateo
 */ 
#define  F_CPU 7372800UL
#include <util/delay.h>
#include <avr/io.h>

void delay(int d) {
	for (int i = 0; i < d; ++i) {
		_delay_ms(1);
	}
}

void debounce() {
	_delay_ms(150);
	GIFR = _BV(INTF0) | _BV(INTF1);
}
