/*	Author: Jongyeon Yoon
 *  Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #7  Exercise #1
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
#include "io.h"
#define SEC 1000

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

typedef enum States {init, wait, pressA0, pressA1, hold_increment, hold_decrement, reset } States;
unsigned char tick_cnt;
unsigned char current_num;

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

int pressToControlValue(int state) {
	unsigned char A0 = (~PINA & 0x01); //get PA0
	unsigned char A1 = (~PINA & 0x02) >> 1; //get PA1

	switch (state) { // Transitions
		case init:
			current_num = 0x00; // initialize current_num to 0
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
				state = tick_cnt > 10 ? hold_increment : pressA0; // if tick count hits 10, initialize count to 0
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
				state = tick_cnt > 10 ? hold_decrement : pressA1;
			}
			else if(!A1) { // releasing the PA1 button
				state = wait;
			}
			break;
		case hold_increment:
			state = pressA0;
			break;
		case hold_decrement:
			state = pressA1;
			break;
		case reset: // both buttons are pressed
			// return to wait state when both buttons are released
			state = (!A0 && !A1) ? wait : reset;
			break;
		default: // cannot come to this state in normal way
			state = init;
			break;
	}
	switch (state) { // Actions
		case init:
			break;
		case wait:
			tick_cnt = 0;
			break;
		case pressA0:
			if(!tick_cnt && current_num < 9) {
				current_num++;
			}
			tick_cnt++; // incrementing or decrementing is operated when cnt value is 0
			break;
		case pressA1:
			if(!tick_cnt && current_num > 0) {
				current_num--;
			}
			tick_cnt++;
			break;
		case hold_increment:
			tick_cnt = 0;
			break;
		case hold_decrement:
			tick_cnt = 0;
			break;
		case reset:
			current_num = 0; // reset current_num to 0
			break;
	}
	
	return state;
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	LCD_init();
	
	TimerSet(SEC / 10); // period goes to 100ms
	TimerOn();
	States state = init;
	
	while(1) {
		// User code (i.e. synchSM calls
		state = pressToControlValue(state);
		LCD_ClearScreen(); // Clear display
		LCD_WriteData(current_num + '0'); // Show current number on LCD
		while(!TimerFlag); //Wait 1 sec
		TimerFlag = 0;
	}
	
	return 0;
}