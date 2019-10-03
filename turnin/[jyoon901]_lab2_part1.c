/*	Author: Jongyeon Yoon
 *      Partner(s) Name:
 *	Lab Section: 021
 *	Assignment: Lab #02  Exercise #1
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
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	while(1) {
		// 1) Read input (both PA1 and PA0)
		tmpA = PINA & 0x03;
		// 2) Perform computation
		// if PA0 is 1 and PA1 is 0, set PB0 = 1, else = 0
		if (tmpA == 0x01) { // True if PA1PA0 is 01
			tmpB = 0x01; // Sets PORTB to 00000001
		} else {
			tmpB = 0x00; // Sets PORTB to 00000000
		}
		PORTB = tmpB;
	}
	return 0;
}
