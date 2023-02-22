
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

// used for storing status after communication with the reader
uint8_t status;

// buffer for fetching information from the reader
uint8_t str[16];
uint8_t trenutna_razina_privilegije = 1;
uint8_t odobreno = 0;
uint8_t rednibroj = 0;
uint8_t j = 0;
uint8_t jj = 0;
char id[10][10];
char trenutni[10];
uint8_t procitano;

uint8_t flag = 0;

int main(void) {
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	
	PORTB = 0x0f;
	DDRB = 	0;
	
	MCUCR = _BV(ISC01) | _BV(ISC11);
	GICR = _BV(INT0) | _BV(INT1);
	sei();
	

	
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
		flag = 0;
		
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
					usart_hex_trenutni(str[i]);
				}
				odobreno = 0;
				for(uint8_t i = 0; i < 10; ++i) {
					for(uint8_t z = 0; z < 10; ++z) {
						if(id[i][z] != trenutni[z]){
							break;
							} else {
							lcd_clrscr();
							lcd_puts("Odobreno");
							odobreno = 1;
							break;
						}
					}
				}
				if(!odobreno){
					lcd_puts("Nema pristup");
				}
				
		   }
		   else {
			 lcd_puts("Error reading serial!\n");
		   }
		   _delay_ms(1500);
		}
		

		
		
		// Test for a tag every 1000ms
		_delay_ms(1000);
		
		
// 		// request information about any tag in range of the antenna 
// 		status = mfrc522_request(PICC_REQALL,str);
// 		lcd_clrscr();
// 	   	lcd_puts("Waiting...\n");
// 		   
// 		if(status == CARD_FOUND) {
// 		   // if card is found, try to fetch card id number
// 			status = mfrc522_get_card_serial(str);
// 			
// 			if(status == CARD_FOUND) {
// 				// send id number (as hex characters) through USART interface
// 				lcd_clrscr();
// 				for(uint8_t i = 0; i < 5; ++i) {
// 					usart_hex(str[i]);
// 				}
// 		   }
// 		   else {
// 			 lcd_puts("Error reading serial!\n");
// 		   }
// 		   _delay_ms(1500);
// 		}
// 	 
// 		// Test for a tag every 1000ms
// 		_delay_ms(1000);
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
	changePrivilegeLevel();
}

ISR(INT1_vect) {
	addNewUser();
}

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

	if (j == 10){
		j = 0;
		rednibroj++;
	}
	id[rednibroj][j] = data;
	j++;
	lcd_putc(data);
}

void usart_putc_trenutni(const unsigned char data) {
	while ( !( UCSRA & _BV(UDRE)) );
	UDR = data;

	if (jj == 10){
		jj = 0;
	}
	trenutni[jj] = data;
	jj++;
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

void usart_hex_trenutni(uint8_t d) {
	char byte = '0';
	// send lower nibble as hex char
	(((d>>4)&0x0F)<=9) ? (byte='0'+((d>>4)&0x0F)) : (byte='A'+ ((d>>4)&0x0F)-0x0A);
	usart_putc_trenutni(byte);
	// send higher nibble as hex char
	((d&0x0F)<=9) ? (byte='0'+ (d&0x0F)) : (byte='A'+ (d&0x0F)-0x0A);
	usart_putc_trenutni(byte);
}

void changePrivilegeLevel(){
	lcd_clrscr();
	lcd_puts("Promijeni razinu\nprivilegija");
	
	while(1){
		if(bit_is_clear(PINB, 0)) {
			lcd_clrscr();
			lcd_puts("1");
			trenutna_razina_privilegije = 1;
 			_delay_ms(1000);
 			break;
		} else if(bit_is_clear(PINB, 1)) {
			lcd_clrscr();
			lcd_puts("2");
			trenutna_razina_privilegije = 2;
			_delay_ms(1000);
			break;
		}  else if(bit_is_clear(PINB, 2)) {
			lcd_clrscr();
			lcd_puts("3");
			trenutna_razina_privilegije = 3;
			_delay_ms(1000);
			break;
		}
	}
	
	_delay_ms(300);
}

void addNewUser(){
	while(!flag){
		lcd_clrscr();
		lcd_puts("Dodaj korisnika");
		lcd_gotoxy(0,1);
		lcd_puts("Skeniraj karticu");
		_delay_ms(500);
		
		status = mfrc522_request(PICC_REQALL,str);
		if(status == CARD_FOUND) {
			// if card is found, try to fetch card id number
			status = mfrc522_get_card_serial(str);
			
			if(status == CARD_FOUND) {
				// send id number (as hex characters) through USART interface
				lcd_clrscr();
				lcd_puts("Dodan korisnik\n");
				for(uint8_t i = 0; i < 5; ++i) {
					usart_hex(str[i]);
				}
				_delay_ms(1000);
				flag = 1;
			}
			else {
				lcd_puts("Error reading serial!\n");
			}
			_delay_ms(1500);
		}
	}
	return;

}
