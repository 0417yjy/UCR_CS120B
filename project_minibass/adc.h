/*
 * ADC_H.h
 * Source: https://www.electronicwings.com/avr-atmega/analog-joystick-interface-with-atmega-16-32
 * Created: 7/28/2016 4:50:16 PM
 *  Author: User
 * Modified: Jongyeon Yoon
 */ 


#ifndef ADC_H_H_
#define ADC_H_H_

#define HORIZON_NEUTRAL 141
#define VERTICAL_NETURAL 151

#define F_CPU 8000000UL					/* Define CPU Frequency e.g. here its 8 Mhz */
#include <avr/io.h>						/* Include AVR std. library file */

void ADC_Init();
int ADC_Read(char);

#endif /* ADC_H_H_ */