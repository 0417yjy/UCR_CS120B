/*
 * ADC_C.c
 * Source: https://www.electronicwings.com/avr-atmega/analog-joystick-interface-with-atmega-16-32
 * Created: 7/28/2016 4:56:54 PM
 *  Author: User
 * Modified: Jongyeon Yoon
 */ 

#include "adc.h"

void ADC_Init()
{
	ADCSRA = 0x87;		/* Enable ADC, fr/128  */
	ADMUX = 0x40;		/* Vref: Avcc, ADC channel: 0 */
}

int ADC_Read(char channel)
{
	int ADC_value;
	
	ADMUX = (0x40) | (channel & 0x07);/* set input channel to read */
	ADCSRA |= (1<<ADSC);	/* start conversion */
	while((ADCSRA &(1<<ADIF))== 0);	/* monitor end of conversion interrupt flag */
	
	ADCSRA |= (1<<ADIF);	/* clear interrupt flag */
	ADC_value = (int)ADCL;	/* read lower byte */
	ADC_value = ADC_value + (int)ADCH*256;/* read higher 2 bits, Multiply with weightage */

	return ADC_value;		/* return digital value */
}