/*	Author: Jongyeon Yoon
 *      Partner(s) Name:
 *	Lab Section: 021
 *	Assignment: Lab #03  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
	DDRA = 0x00; PORTA = 0xFF; // Configure Port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0X00; // Configure port C's 8 pins as outputs
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	unsigned char tmpC = 0x00; // Temporary variable to hold the value of C

	while(1) {
		// 1) Read the rightmost 4 bits of input
		tmpA = PINA & 0x0F;

		// 2) Compute

		//compute fuel levels
		switch(tmpA) {
		case 0: //empty
		tmpC = 0x40; //PORTC : 01000000 (lights only Low fuel)
		break;

		case 1: case 2:
		tmpC = 0x60; //PORTC : 01100000 (lights PC5 and Low fuel)
		break;

		case 3: case 4:
		tmpC = 0x70; //PORTC : 01110000 (lights PC5, PC4, and Low fuel)
		break;

		case 5: case 6:
		tmpC = 0x38; //PORTC : 00111000 (lights PC5..PC3)
		break;

		case 7: case 8: case 9:
		tmpC = 0x3C; //PORTC : 00111100 (lights PC5..PC2)
		break;

		case 10: case 11: case 12:
		tmpC = 0x3E; //PORTC : 00111110 (lights PC5..PC1)
		break;

		case 13: case 14: case 15:
		tmpC = 0x3F; //PORTC : 00111111 (lights PC5..PC0)
		break;

		default: //error: would never come to here
		tmpC = 0x00;
		break;
		}

		//compute "fasten seatbelt" icon
		tmpA = PINA & 0xE0; //Read the PA4..PA6 bits
		if(tmpA == 0x60) {//true if PA4..PA6 is 110 
				//meaning key is in the ignition, driver is seated, belt isn't fastened

			tmpC = tmpC | 0x80; //set PC7 to 1
		}

		// 3) Write output
		PORTC = tmpC;
	}
	return 0;
}
