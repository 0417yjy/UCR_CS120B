/*	Author: Jongyeon Yoon
 *      Partner(s) Name:
 *	Lab Section: 021
 *	Assignment: Lab #03  Exercise #1
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
	DDRB = 0x00; PORTB = 0xFF; // Configure Port B's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0X00; // Configure port C's 8 pins as outputs
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	unsigned char i; //Loop iterator
	unsigned char numOf1s = 0; //Counts number of 1s on ports A and B

	while(1) {
		// 1) Read input & initialize numOf1s
		tmpA = PINA;
		tmpB = PINB;
		numOf1s = 0;

		// 2) Compute
		for(i=0;i<8;i++) { //Iterate 8 times
			if(tmpA & 0x01) { //get the rightmost bit of port A
				//true if rightmost bit of port A is 1
				numOf1s++; //increment the counter variable
			}
			if(tmpB & 0x01) { //perform B as same with A
				numOf1s++;
			}
			tmpA = tmpA >> 1; //right shift both ports once to read the next bit
			tmpB = tmpB >> 1;
		}

		// 3) Write output
		PORTC = numOf1s;
	}
	return 0;
}
