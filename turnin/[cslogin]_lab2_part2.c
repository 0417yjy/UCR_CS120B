/*	Author: Jongyeon Yoon
 *      Partner(s) Name:
 *	Lab Section: 021
 *	Assignment: Lab #02  Exercise #2
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
	unsigned char cntavail;
	while(1) {
		// 1) Read input (PINA's rightmost 4 bits)
		tmpA = PINA & 0x0F;

		// 2) Perform computation
		// cntavail = 4 - value of PINA
		switch(tmpA) {
			//there is no car -> 4 spaces available (0000)
			case 0x00:
			cntavail = 0x04;
			break;

			//there is a car -> 3 spaces available
			//0001 0010 0100 1000
			case 0x01: case 0x02: case 0x04: case 0x08:
			cntavail = 0x03;
			break;
			
			//there are 2 cars -> 2 spaces available
			//0011 0101 1001 0110 1010 1100
			case 0x03: case 0x05: case 0x09: case 0x06: case 0x0A: case 0x0C:
			cntavail = 0x02;
			break;
			
			//there are 3 cars -> 1 space available
			//0111 1011 1101 1110
			case 0x07: case 0x0B: case 0x0D: case 0x0E:
			cntavail = 0x01;
			break;
			
			//there are 4 cars -> no space available (1111)
			case 0x0F:
			cntavail = 0x00;
			break;
		}
		//copy cntavail to PORTC
		PORTC = cntavail;
	}
	return 0;
}
