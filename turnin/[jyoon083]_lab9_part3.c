/*	Author: Jongyeon Yoon
 *  Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #9  Exercise #3
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

// defines frequencies of notes
#define SILENT 0.954
#define E_3 164.81
#define F_3 174.61
#define G_3 196.00
#define A_3 220.00
#define B_3 246.94
#define C_4 261.63
#define D_4 293.66
#define E_4 329.63
#define F_4 349.23
#define G_4 392.00
#define A_4 440.00
#define B_4 493.88
#define C_5 523.25

#define NOTES_NUM 28
#define RESTS_NUM 0

enum States {init, music_off, music_on} state;
typedef enum bool {false, true} bool;

// Period: 60,000 / BPM
// Melody in Canon in C, Johann Pachelbel
float notes_arr[] = {G_4, E_4, F_4, G_4, E_4, F_4, G_4, G_3, A_3, B_3, C_4, D_4, E_4, F_4, E_4, C_4, D_4, E_4, E_3, F_3, G_3, A_3, G_3, F_3, G_3, C_4, B_3, C_4};
unsigned char notes_length_arr[] = {2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, };
unsigned char rest_arr[] = {};
unsigned char rest_length_arr[] = {};

void set_PWM(double frequency) {
	// 0.954 hz is lowest frequency possible with this function,
	// based on settings in PWM_on()
	// Passing in 0 as the frequency will stop the speaker from generating sound
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

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B; // bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0 = 011: pre-scaler /64
	// 00001011: 0x0B
	// So, 8 MHz clock or 8,000,000 / 64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s
	
	// AVR output compare register OCR1A.
	OCR1A = 125; // Timer interrupt will be generated when TCNT1==OCR1A
	// we want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt
	
	//Initialize avr counter
	TCNT1=0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be every _avr_timer_cntcurr milliseconds
	
	//Enable global interrupts
	SREG |= 0x80; // 0x80: 10000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if(_avr_timer_cntcurr == 0) { //results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

bool no_rest = RESTS_NUM ? false : true;

int tick_sm(int state) {
	unsigned char A0 = ~PINA & 0x01;
	static unsigned char note_idx = 0;
	static unsigned char rest_idx = 0;
	static unsigned char note_cnt = 0;
	static bool music_playing = false;
	static bool rest_turn = false;
	
	switch(state) { // Transitions
		case init:
			state = music_off;
			set_PWM(SILENT);
			break;
		case music_off:
			if(A0) {
				state = music_on;
				music_playing = true;
			}
			else {
				state = music_off;
			}
			break;
		case music_on:
			if(music_playing) {
				state = music_on;
			}
			else if (!music_playing && A0) {
				state = music_on;
				set_PWM(SILENT);
			}
			else if (!A0) {
				state = music_off;
				set_PWM(SILENT);
			}
			break;
		default:
			state = init;
	}
	
	switch(state) { // Actions
		case init:
			break;
		case music_off:
			break;
		case music_on: // play music by predetermined arrays
		if(music_playing) {
			if(no_rest) { // no rest mode: if there is no resting in music
				set_PWM(notes_arr[note_idx]);
				note_cnt++;
				if(note_cnt >= notes_length_arr[note_idx]) { // if completed playing this note: play next one
					note_cnt = 0;
					note_idx++;
					if(note_idx == NOTES_NUM) { // if all notes are played: set music_playing false
						music_playing = false;
						note_idx = 0;
					}
				}
			}
			else if(!no_rest) { 
				if(rest_turn) {
					set_PWM(SILENT);
					note_cnt++;
					if(note_cnt >= rest_arr[rest_idx]) {
						rest_turn = false;
						note_cnt = 0;
						rest_idx++;
					}
				}
				else if(!rest_turn) {
					if(note_cnt == 0) { // set frequency at start of notes
						set_PWM(notes_arr[note_idx]);
					}
					note_cnt++;
					if(note_cnt >= notes_length_arr[note_idx]) { // if completed playing this note: play next one
						note_cnt = 0;
						note_idx++;
						if(rest_arr[rest_idx] == note_idx) { // if this is resting turn: set rest_turn true
							rest_turn = true;
						}
						if(note_idx == NOTES_NUM) { // if all notes are played: set music_playing false
							music_playing = false;
							note_idx = 0;
							rest_idx = 0;
						}
					}
				}
			}
		}
		break;
	}
	return state;
}

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	
	TimerSet(280);
	TimerOn();
	state = init;
	PWM_on();
	
	while(1) {
		state = tick_sm(state);
		while(!TimerFlag);
		TimerFlag = 0;
	}
	
	return 0;
}