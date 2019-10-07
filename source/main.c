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
	DDRD = 0x00; PORTD = 0xFF; // Configure Port D's 8 pins as inputs
	DDRB = 0xFE; PORTB = 0X01; // Configure port B's left 7 pins as outputs and rightmost 1 pin as input
	unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	unsigned char tmpD = 0x00; // Temporary variable to hold the value of D
	unsigned short passengerWeight = 0;

	while(1) {
		// 1) Read the inputs
		tmpD = PIND;
		tmpB = PINB & 0x01;

		// 2) Compute

		//compute exact weight and store
		passengerWeight = tmpD + tmpB * 0x100;

		//compute outputs of port B
		if(passengerWeight >= 70) { //if the weight is equal to or above 70 pounds
			tmpB = tmpB | 0x02 //set PB1 to 1
		} else if(passengerWeight < 70 && passengerWeight > 5) {
			tmpB = tmpB | 0x04 //set PB2 to 1
		} else {
			//neither B1 nor B2 should be set if the weight is 5 or less

			//do nothing
		}

		// 3) Write output
		PORTB = tmpB;
	}
	return 0;
}
