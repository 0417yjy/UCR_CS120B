/*	Author: Jongyeon Yoon
 *      Partner(s) Name:
 *	Lab Section: 021
 *	Assignment: Lab #02  Exercise #4
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
	DDRB = 0x00; PORTB = 0xFF; // Configure Port A's 8 pins as inputs
	DDRC = 0x00; PORTC = 0xFF; // Configure Port A's 8 pins as inputs
	DDRD = 0xFF; PORTD = 0X00; // Configure port C's 8 pins as outputs
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	unsigned char tmpC = 0x00; // Temporary variable to hold the value of C
	unsigned char tmpD; // Temporary variable to hold the value of D

	unsigned short sumOfWeights = 0; //stores value of total passenger weight
	signed char diffBetAC = 0; //stores value of difference between A and C

	while(1) {
		// 1) Read inputs and initialize tmpD
		tmpA = PINA;
		tmpB = PINB;
		tmpC = PINC;
		tmpD = 0x00;

		// 2) Perform computation
		//compute sumOfWeights and diffBetAC
		sumOfWeights = tmpA + tmpB + tmpC;
		diffBetAC = tmpA - tmpC;

		//set PD0 to 1 if the cart's total passenger weight exceeds the maximum of 140 kg
		if(sumOfWeights > 140) {
			tmpD = tmpD | 0x01;
		}

		//set PD1 to 1 if the difference between A and C exceeds 80 kg
		if(diffBetAC > 80 || diffBetAC < -80) {
			tmpD = tmpD | 0x02;
		}

		//set PD2-7 to sumOfWeights's 2-7 bits
		//operator can see approximate value of total weights
		//because only sumOfWeight's rightmost 2 bits will be discarded, the difference between approximate value and real value is 3 at most if there aren't values in sumOfWeight's 8-31 bits
		tmpD = tmpD | (sumOfWeights & 0x00FC);

		// 3) Copy values to output Ports
		PORTD = tmpD;
	}
	return 0;
}
