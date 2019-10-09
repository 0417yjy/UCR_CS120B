/*	Author: Jongyeon Yoon
 *  Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #4  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef enum States {releasedPB0, pressedPB0, releasedPB1, pressedPB1 } States;

int pressToChangeLED(int);

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    /* Insert your solution below */
    States state = releasedPB0;
    PORTB = 0x01; // initialize PB0 to 1
    while (1) {
        state = pressToChangeLED(state);
    }
    return 1;
}

int pressToChangeLED(int state) {
    static unsigned char b;
    unsigned char A0 = PINA & 0x01;
    switch (state) { // Transitions
        case releasedPB0:
            state = A0 ? pressedPB1 : releasedPB0;
            break;
	case pressedPB0:
	    state = A0 ? pressedPB0 : releasedPB0;
	    break;
	case releasedPB1:
	    state = A0 ? pressedPB0 : releasedPB1;
	    break;
	case pressedPB1:
	    state = A0 ? pressedPB1 : releasedPB1;
	    break;
        default:
            state = releasedPB0;
            break;
    }
    switch (state) { // Actions
        case releasedPB0:
	    b = 0x01;
	    break;
	case pressedPB0:
	    b = 0x01;
	    break;
	case releasedPB1:
	    b = 0x02;
	    break;
	case pressedPB1:
	    b = 0x02;
	    break;
    }
    PORTB = b;
    return state;
}
