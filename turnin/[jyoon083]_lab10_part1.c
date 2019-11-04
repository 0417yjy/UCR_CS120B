/*	Author: Jongyeon Yoon
 *  Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #10  Exercise #1
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

typedef struct Task {
	int state; // Task's current state
	unsigned long period; // Task period
	unsigned long long elapsedTime; // Time elapsed since last task tick
	int (*TickFct) (int); // Task tick function
	} Task;
	
#define TASK_SIZE 3
Task tasks[TASK_SIZE];

// shared variables
unsigned char threeLEDS;
unsigned char blinkingLED;

enum ThreeLEDsStates {TL_init, TL_light_0, TL_light_1, TL_light_2};
int TickFct_ThreeLEDs (int state) {
	threeLEDS = 0x00;
	
	switch(state) { // Transitions
		case TL_init:
		state = TL_light_0;
		break;
		case TL_light_0:
		state = TL_light_1;
		break;
		case TL_light_1:
		state = TL_light_2;
		break;
		case TL_light_2:
		state = TL_light_0;
		break;
		default:
		state = TL_init;
	}
	
	switch(state) { // Actions
		case TL_init:
		break;
		case TL_light_0:
		threeLEDS = 0x01;
		break;
		case TL_light_1:
		threeLEDS = 0x02;
		break;
		case TL_light_2:
		threeLEDS = 0x04;
		break;
	}
	return state;
};
enum BlinkingLEDsStates {BL_init, BL_light_0, BL_light_1};
int TickFct_BlinkingLEDs (int state) {
	blinkingLED = 0x00;
	
	switch(state) { // Transitions
		case BL_init:
		state = BL_light_0;
		break;
		case BL_light_0:
		state = BL_light_1;
		break;
		case BL_light_1:
		state = BL_light_0;
		break;
		default:
		state = BL_init;
	}
	
	switch(state) { // Actions
		case BL_init:
		break;
		case BL_light_0:
		blinkingLED = 0x01;
		break;
		case BL_light_1:
		blinkingLED = 0x00;
		break;
	}
	return state;
};
enum CombineLEDsStates {CL_init, CL_working};
int TickFct_CombineLEDs (int state) {
	switch(state) {
		case CL_init:
			state = CL_working;
			break;
		case CL_working:
			state = CL_working;
			break;
		default:
			state = CL_init;
			break;
	}
	switch(state) {
		case CL_init:
			break;
		case CL_working:
			PORTB =threeLEDS | (blinkingLED << 3);
			break;
	}
	return state;
};
typedef enum bool {false, true} bool;

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
	unsigned char i;
	for (i=0;i<TASK_SIZE;i++) {
		if(tasks[i].elapsedTime >= tasks[i].period) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += 1;
	}
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

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	
	unsigned char i = 0;
	tasks[i].state = TL_init;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_ThreeLEDs;
	i++;
	tasks[i].state = BL_init;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_BlinkingLEDs;
	i++;
	tasks[i].state = CL_init;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_CombineLEDs;
	
	
	TimerSet(1);
	TimerOn();
	
	while(1) { }
	
	return 0;
}