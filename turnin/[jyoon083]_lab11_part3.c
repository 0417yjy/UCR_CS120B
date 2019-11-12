/*	Author: Jongyeon Yoon
 *  Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #11  Exercise #3
 *	Exercise Description: Changed PORTC and PINC to PORTB and PINB each in keypad.h to operate keypad and LCD concurrently
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "keypad.h"
#include <string.h>
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

#define TASK_SIZE 1
Task tasks[TASK_SIZE];

// shared variables

enum KeypadLCDStates {KL_init, KL_work};
int TickFct_KeypadLCD(int state) {
	unsigned char x;
	
	switch(state) { // Transitions
		case KL_init:
			LCD_ClearScreen();
			state = KL_work;
			break;
		case KL_work:
			state = KL_work;
			break;
		default:
			state = KL_init;
			break;
	}
	
	switch(state) { // Actions
		case KL_init:
			break;
		case KL_work:
			x = GetKeypadKey();
			if(x != '\0') { // if anything in keypad is pressed
				LCD_ClearScreen();
				LCD_WriteData(x); // change LCD display to what was pressed
			}
			break;
	}
	return state;
};

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
	DDRB = 0xF0; PORTB = 0x0F; // PB7..4 outputs init 0s, PB3..0 inputs init 1s
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	LCD_init();
	
	unsigned char i = 0;
	tasks[i].state = KL_init;
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_KeypadLCD;
	
	TimerSet(1);
	TimerOn();
	
	while(1) { }
	
	return 0;
}