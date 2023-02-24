#define F_CPU 7372800UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#include "usart.h"
#include "spi.h"
#include "mfrc522.h"
#include "lcd.h"

void detectReader();

// Used for storing status after communication with the reader
uint8_t status;

// Buffer for fetching information from the reader
uint8_t str[16];

char idAndPrivilegeTable[10][11];
char lastScannedCard[10];
char currentPrivilegeLevel = '1';
uint8_t accessAllowed = 0;
uint8_t addedCardCharPosition = 0;
uint8_t scannedCharPosition = 0;
uint8_t lastScannedCardPosition = 0;
uint8_t scannedCardPosition = 0;
uint8_t checkButtonPressed = 0;
uint8_t privilegeAdded = 0;

//	Function for changing current privilege level required for access
void changePrivilegeLevel(){
	lcd_clrscr();
	lcd_puts("Promijeni razinu\n  privilegija");

	while(1){
		if(bit_is_clear(PINB, 0)) {
			lcd_clrscr();
			lcd_puts(" Unesena razina\n privilegije: 1");
			checkButtonPressed = 1;
			currentPrivilegeLevel = '1';
			_delay_ms(2000);
			break;
			} else if(bit_is_clear(PINB, 1)) {
			lcd_clrscr();
			lcd_puts(" Unesena razina\n privilegije: 2");
			checkButtonPressed = 1;
			currentPrivilegeLevel = '2';
			_delay_ms(2000);
			break;
			}  else if(bit_is_clear(PINB, 2)) {
			lcd_clrscr();
			lcd_puts(" Unesena razina\n privilegije: 3");
			checkButtonPressed = 1;
			currentPrivilegeLevel = '3';
			_delay_ms(2000);
			break;
		}
		if(checkButtonPressed){
			break;
		}
	}
}

//	Function for adding new user and setting up users privilege level
void addNewUser(){
	while(!privilegeAdded){
		lcd_clrscr();
		lcd_puts("Dodaj korisnika");
		lcd_gotoxy(0,1);
		lcd_puts("Skeniraj karticu");
		_delay_ms(1000);
		
		status = mfrc522_request(PICC_REQALL,str);
		if(status == CARD_FOUND) {
			// If card is found, try to fetch card ID number
			status = mfrc522_get_card_serial(str);
			if(status == CARD_FOUND) {
				// Send ID number (as HEX characters) through USART interface
				lcd_clrscr();
				lcd_puts(" Dodan korisnik\n   ");
				for(uint8_t i = 0; i < 5; ++i) {
					usart_hex(str[i]);
				}
				_delay_ms(1500);
				lastScannedCardPosition++;
				lcd_clrscr();
				lcd_puts("Korisnik dodan \nPrivilegija: _");
				//	Waiting for privilege level to be entered
				while(1){
					if(bit_is_clear(PINB, 0)) {
						lcd_gotoxy(13,1);
						lcd_puts("1");
						idAndPrivilegeTable[lastScannedCardPosition-1][10] = '1';
						_delay_ms(1000);
						privilegeAdded = 1;
						break;
						} else if(bit_is_clear(PINB, 1)) {
						lcd_gotoxy(13,1);
						lcd_puts("2");
						idAndPrivilegeTable[lastScannedCardPosition-1][10] = '2';
						_delay_ms(1000);
						privilegeAdded = 1;
						break;
						}  else if(bit_is_clear(PINB, 2)) {
						lcd_gotoxy(13,1);
						lcd_puts("3");
						idAndPrivilegeTable[lastScannedCardPosition-1][10] ='3';
						_delay_ms(1000);
						privilegeAdded = 1;
						break;
					}
				}
			}
			else {
				lcd_puts("Error reading serial!\n");
			}
			_delay_ms(1500);
		}
	} return;
}

//////////	USART FUNKCIJE	//////////
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

	if (addedCardCharPosition == 10){
		addedCardCharPosition = 0;
		lastScannedCardPosition++;
	}
	idAndPrivilegeTable[lastScannedCardPosition][addedCardCharPosition] = data;
	addedCardCharPosition++;
	lcd_putc(data);
}

void usart_putc_trenutni(const unsigned char data) {
	while ( !( UCSRA & _BV(UDRE)) );
	UDR = data;

	if (scannedCharPosition == 10){
		scannedCharPosition = 0;
	}
	lastScannedCard[scannedCharPosition] = data;
	scannedCharPosition++;
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
//////////////////////////////


int main(void) {
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	
	PORTB = 0x0f;
	DDRB = 	0;
	
	MCUCR = _BV(ISC01) | _BV(ISC11);
	GICR = _BV(INT0) | _BV(INT1);
	sei();
	
	// Initialize SPI, UART and MFRC
	spi_init();
	_delay_ms(500);
	
	mfrc522_init();
	_delay_ms(500);
	
	usart_init(9600);
	_delay_ms(500);

	detectReader();
	_delay_ms(2000);
	
	while(1){
		privilegeAdded = 0;
		checkButtonPressed = 0;
		
		// Request information about any tag in range of the antenna
		status = mfrc522_request(PICC_REQALL,str);
		lcd_clrscr();
		lcd_puts("   Skenirajte\n    karticu");
		if(status == CARD_FOUND) {
			// if card is found, try to fetch card id number
			status = mfrc522_get_card_serial(str);

			if(status == CARD_FOUND) {
				// send id number (as hex characters) through USART interface
				lcd_clrscr();
				for(uint8_t i = 0; i < 5; ++i) {
					usart_hex_trenutni(str[i]);
				}
				accessAllowed = 0;
				for(uint8_t i = 0; i < 10; ++i) {
					for(uint8_t z = 0; z < 10; ++z) {
						if(idAndPrivilegeTable[i][z] != lastScannedCard[z]){
							break;
							} else {
							lcd_clrscr();
							scannedCardPosition = i;
							if(currentPrivilegeLevel > idAndPrivilegeTable[scannedCardPosition][10]){
								lcd_puts("Nedovoljna\nprivilegija");
								_delay_ms(2000);
								lcd_clrscr();
								lcd_puts("Vasa razina:");
								lcd_gotoxy(13, 0);
								lcd_putc(idAndPrivilegeTable[scannedCardPosition][10]);
								lcd_gotoxy(0,1);
								lcd_puts("Potrebna:");
								lcd_gotoxy(10, 1);
								lcd_putc(currentPrivilegeLevel);
								}else{
								lcd_puts("Odobreno");
							}
							accessAllowed = 1;
							break;
						}
					}
				}
				if(!accessAllowed){
					lcd_puts(" Nemate pristup");
				}		
			}
			else {
				lcd_clrscr();
				lcd_puts("Error reading serial!\n");
			}
			_delay_ms(1500);
		}
		
		// Test for a tag every 500ms
		_delay_ms(500);
		
	}	
}

//	Detects and prints (sends through UART) reader version
void detectReader() {
	//	Checks the version of the reader
	uint8_t byte = mfrc522_read(VersionReg);
	
	if (byte == 0x92 || byte == 0x91 || byte==0x90) {
		lcd_puts(" RFID uspjesno\n     spojen");
		} else {
		lcd_puts("RFID neuspjesno\n   spojen");
	}
}

ISR(INT0_vect) {
	changePrivilegeLevel();
}

ISR(INT1_vect) {
	addNewUser();
}