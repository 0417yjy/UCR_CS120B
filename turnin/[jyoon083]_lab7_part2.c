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

typedef enum States {init, lights_moving, lights_stopped, wait_reset, release_to_reset, game_won} States;	
unsigned char tick_cnt;
unsigned char score;

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
	unsigned char A0 = ~PINA & 0x01;
	
	switch(state) { // Transitions
		case init:		
		state = lights_moving;
		tick_cnt = 0;
		score = 5;
		LCD_ClearScreen();
		LCD_WriteData(score + '0');
		break;
		case lights_moving:
		if(A0) {
			state = lights_stopped;
			if(tick_cnt % 2) { // if tick_cnt is 1 or 3 (means lights are on middle one)
				score++; // increment score
			}
			else { // else, decrement score
				if(score > 0) {
					score--;
				}
			}
		}
		else if(!A0) {
			state = lights_moving;
			tick_cnt++;
			if(tick_cnt > 3) {
				tick_cnt = 0;
			}
		}
		break;
		case lights_stopped:
		if(score >= 9) {
			state = game_won;
		}
		else {
			state = A0 ? lights_stopped : wait_reset;
		}
		break;
		case wait_reset:
		state = A0 ? release_to_reset : wait_reset;
		break;
		case release_to_reset:
		if(A0) {
			state = release_to_reset;
		}
		else if(!A0) {
			state = lights_moving;
			if(score >= 9) {
				score = 5;
				LCD_ClearScreen();
				LCD_WriteData(score + '0');
			}
		}
		break;
		case game_won:
		if(A0) {
			state = game_won;
		}
		else if(!A0) {
			state = wait_reset;
		}
		break;
		default:
		state = init;
	}
	
	switch(state) { // Actions
		case init:
		break;
		case lights_moving:
		switch(tick_cnt) {
			case 0: PORTB = 0x01; break;
			case 1: PORTB = 0x02; break;
			case 2: PORTB = 0x04; break;
			case 3: PORTB = 0x02; break;
		}
		break;
		case lights_stopped:
		LCD_ClearScreen();
		LCD_WriteData(score + '0');
		break;
		case wait_reset:
		break;
		case release_to_reset:
		tick_cnt = 0;
		break;
		case game_won:
		LCD_ClearScreen();
		LCD_DisplayString(1, "Winner!");
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
	
	TimerSet(300); // period goes to 300ms
	TimerOn();
	States state = init;
	
	while(1) {
		// User code (i.e. synchSM calls
		state = TickSM(state);
		while(!TimerFlag); //Wait 1 sec
		TimerFlag = 0;
	}
	
	return 0;
}