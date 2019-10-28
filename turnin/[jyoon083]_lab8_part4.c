/*	Author: Jongyeon Yoon
 *  Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #8  Exercise #4
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
#define MAX 0x00C0
#define MIN 0x0018

unsigned short adc_value;

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are in
	//		  Free Running Mode a new conversion will trigger whenever
	//		  the previous conversion completes.
}

void photo_light() {
	if(adc_value > (((MAX - MIN) / 9) * 8)) {
		PORTB = 0xFF;
	}
	else if(adc_value > (((MAX - MIN) / 9) * 7)) {
		PORTB = 0xFE;
	}
	else if(adc_value > (((MAX - MIN) / 8) * 6)) {
		PORTB = 0xFC;
	}
	else if(adc_value > (((MAX - MIN) / 8) * 5)) {
		PORTB = 0xF8;
	}
	else if(adc_value > (((MAX - MIN) / 8) * 4)) {
		PORTB = 0xF0;
	}
	else if(adc_value > (((MAX - MIN) / 8) * 3)) {
		PORTB = 0xE0;
	}
	else if(adc_value > (((MAX - MIN) / 8) * 2)) {
		PORTB = 0xC0;
	}
	else {
		PORTB = 0x80;
	}
}

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	ADC_init();
	
	while(1) {
		adc_value = ADC;
		photo_light();
	}
	
	return 0;
}