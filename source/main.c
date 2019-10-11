/*	Author: Jongyeon Yoon
 *      Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #4  Exercise #5
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#define B0 (PORTB & 0x01)

typedef enum States {init, wait, lock, press, unlock } States;

unsigned char buttonSeq[4]; // globalscope array variable that stores button sequence
unsigned char flag; // check if button was stored in buttonSeq (used for guaranteeing that button is stored only once)

int deadBoltSystem(int);

int main(void) {
    unsigned char i; // used in for loop
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;

    /* Insert your solution below */
    for(i=0;i<4;i++) { //initialize buttonSeq
	buttonSeq[i] = 0;
    }
    States state = init; //initialize state
    PORTC = 0; //write wait state to PORTC
    while (1) {
        state = deadBoltSystem(state);
    }
    return 1;
}

int deadBoltSystem(int state) {
    unsigned char i; // used in for loop
    unsigned char A0 = (PINA & 0x01); //get PA0
    unsigned char A1 = (PINA & 0x02); //get PA1
    unsigned char A2 = (PINA & 0x04); //get PA2
    unsigned char A7 = (PINA & 0x80); //get PA7

    switch (state) { // Transitions
	/* All transitions are executed when only one button is pressed */
	case init:
		if(B0 == 0) {
			state = wait;
		}
		else {
			state = init;
		}
		break;
	case wait:
		if(((buttonSeq[0] == '#' && buttonSeq[1] == 'X' && buttonSeq[2] == 'Y' && buttonSeq[3] == 'X') && B0) || A7) {
			// lock if door is unlocked and button sequence is "#XYX"
			state = lock;
		}
		else if((buttonSeq[0] == '#' && buttonSeq[1] == 'X' && buttonSeq[2] == 'Y' && buttonSeq[3] == 'X') && !B0) { 
			// unlock if door is locked and button sequence is "#XYX"
			state = unlock;
		}
		else if(A0 || A1 || A2) { // if any button is pressed, store it into the array
			state = press;
		}
		else {
			state = wait;
		}
		break;
	case lock: // hold present state or go back to wait state
		if(!A7) { // release the A7 button
			state = wait;
		}
		else { // hold all buttons
			state = lock;
		}
		break;
	case press:
		if(!(A0 || A1 || A2)) {
			state = wait;
		}
		else {
			state = press;
		}
		break;
	case unlock: //always go to the wait state
		state = wait;
		break;
	default:
		state = wait;
		break;
    }
    switch (state) { // Actions
	/* Always write the current state to PORTC */
	case init:
		PORTB = 0x00; // initialize PB0 to 0
		PORTC = 0;
		break;
	case wait:
		flag = 0; // get ready to store the char pressed
		PORTC = 1;
		break;
	case lock:
		PORTB = PORTB & 0xFE; // make PB0 to 0: lock
		for(i=0;i<4;i++) { //initialize buttonSeq again
			buttonSeq[i] = 0;
		}
		PORTC = 2;
		break;
	case press:
		if(flag == 0) {
			//shift left once
			buttonSeq[0] = buttonSeq[1];
			buttonSeq[1] = buttonSeq[2];
			buttonSeq[2] = buttonSeq[3];
	
			//push new char in idx 3
			if(A0) {
			  buttonSeq[3] = 'X';
			}
			else if(A1) {
			  buttonSeq[3] = 'Y';
			}
			else if(A2) {
			  buttonSeq[3] = '#';
			}
			flag = 1; // disable storing chars
		}
		PORTC = 3;
		break;
	case unlock:
		PORTB = PORTB | 0x01; // make PB0 to 1: unlock
		for(i=0;i<4;i++) { //initialize buttonSeq again
			buttonSeq[i] = 0;
		}
		PORTC = 4;
		break;
    }
    return state;
}
