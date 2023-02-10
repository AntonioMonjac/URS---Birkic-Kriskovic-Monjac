#define F_CPU 7372800UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "lcd.h"
#include "utility.h"
#include "spi.h"
#include "mfrc522.h"

int array[10][2];

int i = 0, flag = 0;

// used for storing status after communication with the reader
uint8_t status;
	
//varijabla u koju spremamo kod kartice
uint8_t str[16];

void detectReaderVersion();

ISR(INT0_vect) {
	changePrivilegeLevel();
}

ISR(INT1_vect) {
	addNewUser();
}


int main(void) {
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	
	//postavljamo portb kao ulazni kako bi radile tipke
	PORTB = 0x0f;
	DDRB = 	0;
	
	// enable interrupts
	MCUCR = _BV(ISC01) | _BV(ISC11);
	GICR = _BV(INT0) | _BV(INT1);
	sei();
	
	// initialize SPI and MFRC
	spi_init();
	_delay_ms(1000);
	
	mfrc522_init();
	_delay_ms(1000);
	
	detectReaderVersion();
   
	_delay_ms(500);

	char buffer[64];
	
	while(1){
		// request information about any tag in range of the antenna 
		status = mfrc522_request(PICC_REQALL,str);
		lcd_clrscr();
	   	lcd_puts("Scan card");
		flag = 0;
		   
		if(status == CARD_FOUND) {
		   // if card is found, try to fetch card id number
		   lcd_clrscr();

			status = mfrc522_get_card_serial(str);
			
			
// 			for(int j = 0; j<10; j++){
// 				if(array[j][0] == str){
// 					lcd_clrscr();
// 					lcd_puts("U bazi");
// 					_delay_ms(1000);
// 					break;
// 					}else if(j==9) {
// 					lcd_clrscr();
// 					lcd_puts("Tko si ti");
// 					_delay_ms(1000);
// 				}
// 			}			for (int x = 0; x < 3; x++)
			{
				lcd_clrscr();
				lcd_gotoxy(0, 0);
				lcd_puts(array[x][0]);
				_delay_ms(1000);
			}

			if(status == CARD_FOUND) {
				lcd_puts("\n");
		   }
		   else {
			   lcd_clrscr();
			 lcd_puts("Error reading serial!\n");
		   }
		   _delay_ms(1000);
		}
	 
		// Test for a tag every 1000ms
		_delay_ms(1000);
   }
}

void detectReaderVersion() {
	//check version of the reader
	uint8_t byte = mfrc522_read(VersionReg);
	
	if (byte == 0x92) {
		lcd_clrscr();
		lcd_puts("Reader detected");
		lcd_gotoxy(0, 1);
		lcd_puts("Ready to Scan");

	} else {
		lcd_clrscr();
		lcd_puts("No reader found\n");
	}
	_delay_ms(1000);
}

void changePrivilegeLevel(){

}

void addNewUser(){
	while(!flag){
		lcd_clrscr();
		lcd_puts("New user \nScan card");
		// u varijablu status se sprema "CARD FOUND" ako je kartica o?itana
		status = mfrc522_request(PICC_REQALL,str);
		char test[16];
		itoa(i, test, 8);
		array[i][0] = test;
		if(status == CARD_FOUND) {
			lcd_clrscr();
			lcd_puts("User added \nPrivilege: _");
			//petlja koja ?eka na pritisak neke od tipki za postavljanje privilegije
			while(1){
				if(bit_is_clear(PINB, 0)) {
					lcd_gotoxy(11,1);
					lcd_puts("1");
					//dodati provjeru ako je kartica vec dodana kako se nebi dodala dva puta
					//int test = str;
					//array[i][0] = str;
					array[i][1] = 1;
					i++;
 					_delay_ms(1000);
					 flag = 1;					
 					break;					
				} else if(bit_is_clear(PINB, 1)) {
					lcd_gotoxy(11,1);
					lcd_puts("2");
					//dodati provjeru ako je kartica vec dodana kako se nebi dodala dva puta
					array[i][0] = str;
					array[i++][1] = 2;
					_delay_ms(1000);
					flag = 1;
					break;			
				}  else if(bit_is_clear(PINB, 2)) {
					lcd_gotoxy(11,1);
					lcd_puts("3");
					//dodati provjeru ako je kartica vec dodana kako se nebi dodala dva puta
					array[i][0] = str;
					array[i++][1] = 3;
					_delay_ms(1000);
					flag = 1;
					break;
				}
			}
			break;
		}
	}

}


