/*	Author: Jongyeon Yoon
 *  Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #9  Exercise #2
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

enum States {init, turn_off_press, turn_off_release, turn_on_press, turn_on_release, scale_up, scale_down} state;

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
		// 0.954 is smallest frequency that will not result in overflow
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
	static unsigned char note_idx = 0;
	static float note_arr[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
	
	switch(state) { // Transitions
		case init:
			state = turn_off_release;
			break;
		case turn_off_release:
			state = A0 ? turn_on_press : turn_off_release;
			break;
		case turn_on_press:
			state = A0 ? turn_on_press : turn_on_release;
			break;
		case turn_on_release:
			if(A0) {
				state = turn_off_press;
			}
			else if(A1) {
				state = scale_up;
				if(note_idx < 7) {
					note_idx++;
				}
			}
			else if(A2) {
				state = scale_down;
				if(note_idx > 0) {
					note_idx--;
				}
			}
			else {
				state = turn_on_release;
			}
			break;
		case turn_off_press:
			state = A0 ? turn_off_press : turn_off_release;
			break;
		case scale_up:
			state = A1 ? scale_up : turn_on_release;
			break;
		case scale_down:
			state = A2 ? scale_down : turn_on_release;
			break;
		default:
			state = init;
	}
	
	switch(state) { // Actions
		case init:
			break;
		case turn_off_release:
			set_PWM(0.954); // set lowest frequency possible
			break;
		case turn_on_press:
		case turn_on_release:
		case scale_up:
		case scale_down:
			set_PWM(note_arr[note_idx]);
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