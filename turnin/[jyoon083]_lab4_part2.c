/*	Author: Jongyeon Yoon
 *      Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #4  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef enum States {init, wait, pressA0, pressA1, reset } States;
typedef enum boolean {false, true} boolean;

int pressToControlPortCValue(int, boolean*);

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x07;

    /* Insert your solution below */
    States state = init;
    boolean flag = false; // flag shows whether incrementing or decrementing is executed
			// both incrementing and decrementing needs that the flag is 0
			// each execution makes the flag 1
    //PORTC = 0x07; // initialize PORTC to 7
    while (1) {
        state = pressToControlPortCValue(state, &flag);
    }
    return 1;
}

int pressToControlPortCValue(int state, boolean *flag) {
    //unsigned char PORTC = PORTC;
    unsigned char A0 = (PINA & 0x01); //get PA0
    unsigned char A1 = (PINA & 0x02) >> 1; //get PA1

    switch (state) { // Transitions
	case init:
		PORTC = 0x07; // initialize PORTC to 7
		*flag = false;
		state = wait;
		break;
	case wait:
		if(A0 && A1) { // both buttons are depressed
			state = reset;
		}
		else if(A0) { // button at PA0 are depressed
			state = pressA0;
		}
		else if(A1) { // button at PA1 are depressed
			state = pressA1;
		}
		else { // both buttons are waiting for press
			state = wait;
		}
		break;
	case pressA0: // button at PA0 are pressed
		if(A1) { // both buttons are pressed
			state = reset;
		}
		else if(A0) { // holding the PA0 button
			state = pressA0;
		}
		else if(!A0) { // releasing the PA0 button
			state = wait;
		}
		break;
	case pressA1: // button at PA1 are pressed
		if(A0) { // both buttons are pressed
			state = reset;
		}
		else if(A1) { // holding the PA1 button
			state = pressA1;
		}
		else if(!A1) { // releasing the PA1 button
			state = wait;
		}
		break;
	case reset: // both buttons are pressed
		// return to wait state when both buttons are released
		state = (!A0 && !A1) ? wait : reset;
		break;
	default: // cannot come to this state in normal way
		break;
    }
    switch (state) { // Actions
	case init:
		PORTC = 0x07; // initialize PORTC to 7
		*flag = false;
		break;
	case wait:
		*flag = false;
		break;
	case pressA0:
		if(PORTC < 9 && (*flag) == false) {
			PORTC++;
			*flag = true;
		}
		break;
	case pressA1:
		if(PORTC > 0 && (*flag) == false) {
			PORTC--;
			*flag = true;
		}
		break;
	case reset:
		PORTC = 0; // reset PORTC to 0
		break;
    }
    //PORTC = tmpC;
    return state;
}
