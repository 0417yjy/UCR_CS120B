/*	Author: Jongyeon Yoon
 *      Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #4  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef enum States {wait, lock, unlockSeq1, unlockSeq2, unlock } States;
//typedef enum boolean {false, true} boolean;

int deadBoltSystem(int);

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;

    /* Insert your solution below */
    States state = wait; //initialize state
    PORTC = 0; //write wait state to PORTC
    while (1) {
        state = deadBoltSystem(state);
    }
    return 1;
}

int deadBoltSystem(int state) {
    unsigned char A0 = (PINA & 0x01); //get PA0
    unsigned char A1 = (PINA & 0x02) >> 1; //get PA1
    unsigned char A2 = (PINA & 0x04) >> 2; //get PA2
    unsigned char A7 = (PINA & 0x80) >> 7; //get PA7

    switch (state) { // Transitions
	/* All transitions are executed when only one button is pressed */
	case wait:
		if(A7 && !(A0 || A1 || A2)) { // A7 is pressed
			state = lock; // lock the door
		}
		else if(A2 && !(A0 || A1 || A7)) { // A2(#) is pressed
			state = unlockSeq1; // proceed to next step for unlocking
		}
		else {
			state = wait;
		}
		break;
	case lock: // hold present state or go back to wait state
		if(A7) { // hold the button
			state = lock;
		}
		else if(!A7) { // release the button
			state = wait;
		}
		break;
	case unlockSeq1: // hold present state or proceed to unlockSeq2 state
		if(!A0 && !(A0 || A1 || A7)) { //release the button at A0
			state = unlockSeq2;
		}
		else { // holding the button or other buttons are pressed
			state = unlockSeq1;
		}
		break;
	case unlockSeq2: // press A1 to unlock
			// otherwise, lock the door(A7) or reset(A0 or A1)
		if(A1 && !(A0 || A2 || A7) && !(PORTB & 0x01)) { // if door is locked, unlock the door
			state = unlock;
		}
		else if((A7 && !(A0 || A1 || A2)) || (A1 && !(A0 || A2 || A7) && (PORTB & 0x01))) {
			// if door is unlocked, lock the door
			state = lock;
		}
		else if((A0 && !(A1 || A2 || A7)) || (A2 && !(A0 || A1 || A7))) { // reset
			state = wait;
		}
		else { // press multiple buttons simultaneously or no buttons
			state = unlockSeq2;
		}
		break;
	case unlock: // hold present state or go back to wait state
		if(A1) { // hold the button
			state = unlock;
		}
		else if(!A1) { // release the button
			state = wait;
		}
		break;
	default: // cannot come to this state in normal way
		break;
    }
    switch (state) { // Actions
	/* Always write the current state to PORTC */
	case wait:
		PORTC = 0;
		break;
	case lock:
		PORTB = PORTB & 0xFE; // make PB0 to 0: lock
		PORTC = 1;
		break;
	case unlockSeq1:
		PORTC = 2;
		break;
	case unlockSeq2:
		PORTC = 3;
		break;
	case unlock:
		PORTB = PORTB | 0x01; // make PB0 to 1: unlock
		PORTC = 4;
		break;
    }
    return state;
}
