
#define F_CPU 7372800UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#include "utility.h"
#include "usart.h"
#include "spi.h"
#include "mfrc522.h"
#include "lcd.h"

void detectReader();

//uint8_t tekst;
uint8_t procitano;

int main(void) {
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	
	MCUCR = _BV(ISC01) | _BV(ISC11);
	GICR = _BV(INT0) | _BV(INT1);
	sei();

	char id[10];
	

	
	// used for storing status after communication with the reader
	uint8_t status;
	
	// buffer for fetching information from the reader
	uint8_t str[16];
	
	// initialize SPI, UART and MFRC
	spi_init();
	_delay_ms(1000);
	
	mfrc522_init();
	_delay_ms(1000);
	
	usart_init(9600);
	_delay_ms(500);

	// enable interrupts
	
	detectReader();
   
	_delay_ms(3000);
   
   
	char buffer[64];
	while(1){
		// request information about any tag in range of the antenna 
		status = mfrc522_request(PICC_REQALL,str);
		lcd_clrscr();
	   	lcd_puts("Waiting...\n");
		   
		if(status == CARD_FOUND) {
		   // if card is found, try to fetch card id number
			status = mfrc522_get_card_serial(str);
			
			if(status == CARD_FOUND) {
				// send id number (as hex characters) through USART interface
				lcd_clrscr();
				for(uint8_t i = 0; i < 5; ++i) {
					usart_hex(str[i]);
				}
				procitano = lcd_read(1);
		   }
		   else {
			 lcd_puts("Error reading serial!\n");
		   }
		   _delay_ms(1500);
		}
	 
		// Test for a tag every 1000ms
		_delay_ms(1000);
   }
	
	
}

// Detects and prints(sends through UART) reader version
void detectReader() {
	//check version of the reader
	uint8_t byte = mfrc522_read(VersionReg);
	
	if (byte == 0x92 || byte == 0x91 || byte==0x90) {
		lcd_puts("RFID uspjesno");
	} else {
		lcd_puts("RFID nije");
	}
}

ISR(INT0_vect) {
	lcd_clrscr();
	lcd_puts(procitano);
	_delay_ms(500);
}

ISR(INT1_vect) {

}


