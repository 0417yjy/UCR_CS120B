/*	Author: Jongyeon Yoon
 *  Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

typedef enum States {init, light_0, light_1, light_2} States;

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

int TickSM(int state) {
	unsigned char tmpB = 0x00;
	
	switch(state) { // Transitions
		case init:
			state = light_0;
			break;
		case light_0:
			state = light_1;
			break;
		case light_1:
			state = light_2;
			break;
		case light_2:
			state = light_0;
			break;
		default:
			state = init;
	}
	
	switch(state) { // Actions
		case init:
			break;
		case light_0:
			tmpB = 0x01;
			break;
		case light_1:
			tmpB = 0x02;
			break;
		case light_2:
			tmpB = 0x04;
			break;
	}
	PORTB = tmpB;
	return state;
}

int main(void) {
	DDRB = 0xFF; // Set port B to output
	PORTB = 0x00; // Init port B to 0s
	TimerSet(1000);
	TimerOn();
	States state = init;
	while(1) {
		// User code (i.e. synchSM calls
		state = TickSM(state);
		while(!TimerFlag); //Wait 1 sec
		TimerFlag = 0;
		// This example just illustrates the use of the ISR and flag
	}
}