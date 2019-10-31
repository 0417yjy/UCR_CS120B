/*	Author: Jongyeon Yoon
 *  Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include <avr/interrupt.h>

enum States {init, silent, playC, playD, playE} state;

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if(!frequency) {TCCR3B &= 0x08;} // stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
			
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overfulow
		if(frequency < 0.954) { OCR3A = 0xFFFF; }
			
		// prevents OCR3A from underflowing, using prescaler 64
		// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
		// COM3A: Toggle PB3 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
		// WGM32: When counter (TCNT3) matches OCR3A, reset counter
		// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

int tick_sm(int state) {
	unsigned char A0 = ~PINA & 0x01;
	unsigned char A1 = ~PINA & 0x02;
	unsigned char A2 = ~PINA & 0x04;
	
	switch(state) { // Transitions
		case init:
			state = silent;
			break;
		case silent:
			if(A0 && !A1 && !A2) {
				state = playC;
			}
			else if(!A0 && A1 && !A2) {
				state = playD;
			}
			else if(!A0 && !A1 && A2) {
				state = playE;
			}
			else {
				state = silent;
			}
			break;
		case playC:
			if(A0 && !A1 && !A2) {
				state = playC;
			}
			else {
				state = silent;
			}
			break;
		case playD:
			if(!A0 && A1 && !A2) {
				state = playD;
			}
			else {
				state = silent;
			}
			break;
		case playE:
			if(!A0 && !A1 && A2) {
				state = playE;
			}
			else {
				state = silent;
			}
			break;
		default:
			state = init;
	}
	
	switch(state) { // Actions
		case silent:
			set_PWM(0.954); // play 0.954hz
			break;
		case playC:
			set_PWM(261.63); // play C4
			break;
		case playD:
			set_PWM(293.66); // play D4
			break;
		case playE:
			set_PWM(329.63); // play E4
			break;
	}
	return state;
}

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	
	state = init;
	PWM_on();
	while(1) {
		state = tick_sm(state);
	}
	
	return 0;
}