/*	Author: Jongyeon Yoon
 *      Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #5  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>

typedef enum States {init, right_order, right_operation, left_order, left_operation } States;
typedef enum boolean {false, true} boolean;
unsigned char press_cnt;

int tick_festive_led(int);

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;

    /* Insert your solution below */
    States state = init;
    while (1) {
        state = tick_festive_led(state);
    }
    return 1;
}

int tick_festive_led(int state) {
    unsigned char A0 = (~PINA & 0x01); //get PA0

    switch (state) { // Transitions
	case init:
		state = right_order;
		break;
	case right_order:
		if(press_cnt > 7) { // if number of pressing is above 7, reset and invert the order next time
			state = left_order;
			PORTC = 0;
			press_cnt = 0;
		}
		else if(A0) { // press button
			state = right_operation;
			press_cnt++;
		}
		else { // waiting
			state = right_order;
		}
		break;
	case right_operation:
		if(A0) { // holding
			state = right_operation;
		}
		else if(!A0) { // release button
			state = right_order;
		}
		break;
	case left_order:
		if(press_cnt > 7) { // if number of pressing is above 7, reset and invert the order next time
			state = right_order;
			PORTC = 0;
			press_cnt = 0;
		}
		else if(A0) { // press button
			state = left_operation;
			press_cnt++;
		}
		else { // waiting
			state = left_order;
		}
		break;
	case left_operation:
		if(A0) { // holding
			state = left_operation;
		}
		else if(!A0) { // release button
			state = left_order;
		}
		break;
	default: // cannot come to this state in normal way
		state = init;
		PORTC = 0;
		press_cnt = 0;
		break;
    }
    switch (state) { // Actions
	case init:
		PORTC = 0x00;
		break;
	case right_order: // light up by the count number
		switch(press_cnt) {
			case 1:
				PORTC = 0x01;
				break;
			case 2:
				PORTC = 0x03;
				break;
			case 3:
				PORTC = 0x07;
				break;
			case 4:
				PORTC = 0x0F;
				break;
			case 5:
				PORTC = 0x1F;
				break;
			case 6:
				PORTC = 0x3F;
				break;
			case 7:
				PORTC = 0x7F;
				break;
			default:
				break;
		}
		break;
	case right_operation: // do nothing
		break;
	case left_order:
		switch(press_cnt) { // light up by the count number
			case 1:
				PORTC = 0x40;
				break;
			case 2:
				PORTC = 0x60;
				break;
			case 3:
				PORTC = 0x70;
				break;
			case 4:
				PORTC = 0x78;
				break;
			case 5:
				PORTC = 0x7C;
				break;
			case 6:
				PORTC = 0x7E;
				break;
			case 7:
				PORTC = 0x7F;
				break;
			default:
				break;
		}
		break;
	case left_operation: // do nothing
		break;
    }
    return state;
}