/*	Author: Jongyeon Yoon
 *      Partner(s) Name: Weifeng
 *	Lab Section: 021
 *	Assignment: Lab #03  Exercise #4
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
	DDRB = 0xFF; PORTB = 0X00; // Configure Port B's 8 pins as outputs
	DDRC = 0xFF; PORTC = 0X00; // Configure Port C's 8 pins as outputs

	//temporary stores port A, B, C's value
	unsigned char tmpA = 0x00;
	unsigned char tmpB = 0x00;
	unsigned char tmpC = 0x00;

	while(1) {
		// 1) Read the inputs
		tmpA = PINA;

		// 2) Compute
		tmpB = tmpA >> 4; //take the upper nibble of PINA and map it to the lower one of PORTB
		tmpC = tmpA << 4; //take the lower nibble of PINA and map it to the upper one of PORTC

		// 3) Write output
		PORTB = tmpB;
		PORTC = tmpC;
	}
	return 0;
}
