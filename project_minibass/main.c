/*	Author: Jongyeon Yoon
 *	Lab Section:021
 *	Description: My custom project's main source file.
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include <avr/interrupt.h>
#include "tasks.h"
#define SYSTEM_PERIOD 2

/******* Timer variables and functions start *******/ 
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
		tasks[i].elapsedTime += SYSTEM_PERIOD;
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
/******* Timer variables and functions end *******/

int main(void) {
	DDRA = 0x00; PORTA = 0xFF; // set PORTA as inputs (A0..3 are strings, A4..7 are fret 14, 15, 16, 17)
	DDRB = 0xFF; PORTB = 0x00; // set PORTB as outputs (B0..5, B7 are in 7-segment display, B6 is for the speaker)
	DDRC = 0x00; PORTC = 0xFF; // set PORTC as inputs (C0..7 are fret 6..13)
	DDRD = 0x07; PORTD = 0xF8; // D0..2 are outputs, D3..7 are inputs as fret 1..5
	
	// initialize tuning arrays
	unsigned char i = 0;
	strings_tuning[i].octave = 1;
	strings_tuning[i].letter_idx = 4;
	i++;
	strings_tuning[i].octave = 1;
	strings_tuning[i].letter_idx = 9;
	i++;
	strings_tuning[i].octave = 2;
	strings_tuning[i].letter_idx = 2;
	i++;
	strings_tuning[i].octave = 2;
	strings_tuning[i].letter_idx = 7;
	i = 0;
	
	tasks[i].state = SM_init;
	tasks[i].period = 500;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_SwitchMode;
	i++;
	tasks[i].state = BI_init;
	tasks[i].period = 2;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_ButtonInput;
	i++;
	tasks[i].state = DAP_init;
	tasks[i].period = 2;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_DisplayAndPlay;
	i++;
	tasks[i].state = SF_init;
	tasks[i].period = 50;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_SelectFunction;
	i++;
	tasks[i].state = M_init;
	tasks[i].period = 50;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_metrOnome;
	i++;
	tasks[i].state = BL_init;
	tasks[i].period = SYSTEM_PERIOD;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_BlinkLED;
	i++;
	
	PWM_on();
	TimerSet(SYSTEM_PERIOD);
	TimerOn();
	
	while(1) { }
	
	return 0;
}