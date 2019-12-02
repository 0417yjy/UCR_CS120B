/*
 * declarations.h
 *
 * Created: 2019-11-18 오후 2:36:27
 *  Author: Jongyeon Yoon
 */ 


#ifndef DECLARATIONS_H_
#define DECLARATIONS_H_

#define FP 0
#define PROG 1

#define FN_NOTSELECTED 0
#define FN_METRO 1
#define FN_TUNING 2
#define FN_OCTAVE 3
#define FN_SAVE 4
#define FN_LOAD 5
#define FN_JOYFIGURE 6

#define STR_1 (~PINA & 0x01)
#define STR_2 (~PINA & 0x02)
#define STR_3 (~PINA & 0x04)
#define STR_4 (~PINA & 0x08)


#define FRET_1 (~PIND & 0x08)
#define FRET_2 (~PIND & 0x10)
#define FRET_3 (~PIND & 0x20)
#define FRET_4 (~PIND & 0x40)
#define FRET_5 (~PIND & 0x80)
#define FRET_6 (~PINC & 0x01)
#define FRET_7 (~PINC & 0x02)
#define FRET_8 (~PINC & 0x04)
#define FRET_9 (~PINC & 0x08)
#define FRET_10 (~PINC & 0x10)
#define FRET_11 (~PINC & 0x20)
#define FRET_12 (~PINC & 0x40)
#define FRET_13 (~PINC & 0x80)
#define FRET_14 (~PINA & 0x80)
#define FRET_15 (~PINA & 0x40)
#define FRET_16 (~PINA & 0x20)
#define FRET_17 (~PINA & 0x10)

#define PITCH_C1 32.70
#define PITCH_CS1 34.65
#define PITCH_D1 36.71
#define PITCH_DS1 38.89
#define PITCH_E1 41.20
#define PITCH_F1 43.65
#define PITCH_FS1 46.25
#define PITCH_G1 49.00
#define PITCH_GS1 51.91
#define PITCH_A1 55.00
#define PITCH_AS1 58.27
#define PITCH_B1 61.74

/*#define PITCH_C1 33
#define PITCH_CS1 35
#define PITCH_D1 37
#define PITCH_DS1 38
#define PITCH_E1 41
#define PITCH_F1 44
#define PITCH_FS1 46
#define PITCH_G1 49
#define PITCH_GS1 52
#define PITCH_A1 55
#define PITCH_AS1 58
#define PITCH_B1 62*/

#define METRO_LED_ON() (PORTD = PORTD | 0x04)
#define METRO_LED_OFF() (PORTD = PORTD & 0xFB)
#define SHARP_LED_ON() (PORTD = PORTD | 0x02)
#define SHARP_LED_OFF() (PORTD = PORTD & 0xFD)

#define SEVEN_A_ON() (PORTB &= 0xDF)
#define SEVEN_A_OFF() (PORTB |= 0x20)
#define SEVEN_B_ON() (PORTB &= 0xEF)
#define SEVEN_B_OFF() (PORTB |= 0x10)
#define SEVEN_C_ON() (PORTB &= 0xFD)
#define SEVEN_C_OFF() (PORTB |= 0x02)
#define SEVEN_D_ON() (PORTB &= 0xFB)
#define SEVEN_D_OFF() (PORTB |= 0x04)
#define SEVEN_E_ON() (PORTB &= 0xF7)
#define SEVEN_E_OFF() (PORTB |= 0x08)
#define SEVEN_F_ON() (PORTB &= 0x7F)
#define SEVEN_F_OFF() (PORTB |= 0x80)
#define SEVEN_G_ON() (PORTD &= 0xFE)
#define SEVEN_G_OFF() (PORTD |= 0x01)
#define SEVEN_DOT_ON() (PORTB &= 0xFE)
#define SEVEN_DOT_OFF() (PORTB |= 0x01)

/* #define PITCH_C2 65.41
#define PITCH_CS2 69.30
#define PITCH_D2 73.42
#define PITCH_DS2 77.78
#define PITCH_E2 82.41
#define PITCH_F2 87.31
#define PITCH_FS2 92.50 */

#endif /* DECLARATIONS_H_ */