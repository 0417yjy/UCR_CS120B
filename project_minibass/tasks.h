/*
 * tasks.h
 * Description: Source of State Machines
 * Created: 2019-11-18 14:19:00
 *  Author: Jongyeon Yoon
 */ 
#include "declarations.h"

typedef struct Task {
	int state; // Task's current state
	unsigned long period; // Task period
	unsigned long long elapsedTime; // Time elapsed since last task tick
	int (*TickFct) (int); // Task tick function
} Task;

typedef enum bool {false, true} bool;

#define TASK_SIZE 3
Task tasks[TASK_SIZE];

// shared variables
bool mod;
unsigned char pressed_fret_num;
unsigned char pressed_str_num;

/* State Machine Templates

enum _States { }; 
int TickFct_ (int state) {
	switch(state) { // Transitions

	}
	
	switch(state) { // Actions

	}
	return state;
};

*/

enum SwitchModeStates {SM_init, SM_playing_mode, SM_hold_to_prog, SM_programming_mode };
int TickFct_SwitchMode (int state) {
	static unsigned char cnt;
	switch(state) { // Transitions
		case SM_init:
		state = SM_playing_mode;
		cnt = 0;
		break;
		
		case SM_playing_mode:
		state = (FRET_12 && FRET_13) ? SM_hold_to_prog : SM_playing_mode;
		break;
		
		case SM_hold_to_prog:
		if(cnt > 3) {
			state = SM_programming_mode;
			mod = PROG;
			cnt = 0;
		}
		else if(FRET_12 && FRET_13) {
			state = SM_hold_to_prog;
		}
		else {
			state = SM_playing_mode;
			cnt = 0;
		}
		break;
		
		case SM_programming_mode:
		state = (mod == PROG) ? SM_programming_mode : SM_playing_mode;
		break;
		
		default:
		state = SM_init;
	}
	
	switch(state) { // Actions
		case SM_init:
		break;
		
		case SM_playing_mode:
		mod = FP;
		break;
		
		case SM_hold_to_prog:
		cnt++;
		break;
		
		case SM_programming_mode:
		break;
	}
	
	return state;
}

bool is_str_pressed() {
	if(STR_1 || STR_2 || STR_3 || STR_4) {
		return true;
	}
	else {
		return false;
	}
}

bool is_fret_pressed() {
	if(FRET_1 || FRET_2 || FRET_3 || FRET_4 || FRET_5 || FRET_6 || FRET_7 || FRET_8 || FRET_9 || FRET_10 || FRET_11 || FRET_12 || FRET_13 || FRET_14 || FRET_15 || FRET_16 || FRET_17) {
	//if((~PIND & 0xF8) || (~PINC) || (~PINA & 0xF0)) {
		return true;
	}
	else {
		return false;
	}
}

unsigned char get_fret_num()
{
	// could be optimized
	unsigned char fret_num = 0;
	
	if(FRET_1) {
		fret_num = 1;
	}
	if(FRET_2) {
		fret_num = 2;
	}
	if(FRET_3) {
		fret_num = 3;
	}
	if(FRET_4) {
		fret_num = 4;
	}
	if(FRET_5) {
		fret_num = 5;
	}
	if(FRET_6) {
		fret_num = 6;
	}
	if(FRET_7) {
		fret_num = 7;
	}
	if(FRET_8) {
		fret_num = 8;
	}
	if(FRET_9) {
		fret_num = 9;
	}
	if(FRET_10) {
		fret_num = 10;
	}
	if(FRET_11) {
		fret_num = 11;
	}
	if(FRET_12) {
		fret_num = 12;
	}
	if(FRET_13) {
		fret_num = 13;
	}
	if(FRET_14) {
		fret_num = 14;
	}
	if(FRET_15) {
		fret_num = 15;
	}
	if(FRET_16) {
		fret_num = 16;
	}
	if(FRET_17) {
		fret_num = 17;
	}
	return fret_num;
}

unsigned char get_str_num(unsigned char old_str_num) {
	unsigned char str_num = (~PINA & 0x0F) << 4;
	unsigned char i;
	// if 2 or more strings are pressed simultaneously, return higher string
	for(i=0;i<4;i++) {
		if(str_num & 0x80) {
			switch(i) {
				case 0:
				return 4;
				case 1:
				return 3;
				case 2:
				return 2;
				case 3:
				return 1;
			}
		}
		str_num = str_num << 1;
	}
	return 0;
}

enum ButtonInputStates { BI_init, BI_idle, BI_fingering, BI_playing };
int TickFct_ButtonInput (int state) {
	switch(state) { // Transitions
		case BI_init:
		state = BI_idle;
		pressed_fret_num = 0;
		pressed_str_num = 0;
		break;
		
		case BI_idle:
		if(is_str_pressed()) {
			state = BI_playing;
		}
		else if(is_fret_pressed()) {
			state = BI_fingering;
		}
		else {
			state = BI_idle;
		}
		break;
		
		case BI_fingering:
		if(is_str_pressed()) {
			state = BI_playing;
		}
		else if(is_fret_pressed()) {
			state = BI_fingering;
		}
		else {
			state = BI_idle;
		}
		break;
		
		case BI_playing:
		if(is_str_pressed()) {
			state = BI_playing;
		}
		else if(is_fret_pressed()) {
			state = BI_fingering;
			pressed_str_num = 0;
		}
		else {
			state = BI_idle;
			pressed_fret_num = 0;
			pressed_str_num = 0;
		}
		break;
		
		default:
		state = BI_init;
	}
	
	switch(state) { // Actions
		case BI_init:
		break;
		
		case BI_idle:
		break;
		
		case BI_fingering:
		pressed_fret_num = get_fret_num();
		break;
		
		case BI_playing:
		pressed_fret_num = get_fret_num();
		pressed_str_num = get_str_num(pressed_str_num);
		break;
	}
	return state;
}

typedef struct Pitch {
	unsigned char octave;
	unsigned char letter_idx;
	} Pitch;
const float pitches_octave1[12] = { PITCH_C1, PITCH_CS1, PITCH_D1, PITCH_DS1, PITCH_E1, PITCH_F1, PITCH_FS1, PITCH_G1, PITCH_GS1, PITCH_A1, PITCH_AS1, PITCH_B1 };
Pitch strings_tuning[4];
// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
// set_PWM goes to tasks.h

/******* PWM variables and functions start *******/

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A: Toggle PB3 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if(!frequency) {TCCR3B &= 0x08;} // stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overfulow
		if(frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR3A from underflowing, using prescaler 64
		// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }
	}
}
void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

/******* PWM variables and functions end *******/


void play_note(unsigned char fret, unsigned char str) {
	double desired_frequency;
	unsigned char tmp_letter_idx = strings_tuning[str - 1].letter_idx + fret;
	unsigned char tmp_octave = strings_tuning[str - 1].octave;
	
	if(tmp_letter_idx > 11) {
		tmp_octave = tmp_octave + (tmp_letter_idx / 12);
		tmp_letter_idx %= 12;
	}
	
	desired_frequency = (double) tmp_octave * pitches_octave1[tmp_letter_idx];
	set_PWM(desired_frequency);
}
void display_7segments_char(char ch) {
	// initialize 7-segments display
	//PORTB &= 0x40;
	//PORTD &= 0xFE;
	PORTB |= 0xBF;
	PORTD |= 0x01;
	
	switch(ch) {
		case 'A':
		// turn on abcefg
		SEVEN_A_ON();
		SEVEN_B_ON();
		SEVEN_C_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		case 'b':
		// turn on cdefg
		SEVEN_C_ON();
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		case 'C':
		// turn on adef
		SEVEN_A_ON();
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		break;
		
		case 'd':
		// turn on bcdeg
		SEVEN_B_ON();
		SEVEN_C_ON();
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_G_ON();
		break;
		
		case 'E':
		// turn on adefg
		SEVEN_A_ON();
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		case 'F':
		// turn on aefg
		SEVEN_A_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		case 'G':
		// turn on acdef
		SEVEN_A_ON();
		SEVEN_C_ON();
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		break;
		
		case 'O':
		// turn on abcdef
		SEVEN_A_ON();
		SEVEN_B_ON();
		SEVEN_C_ON();
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		break;
		
		case 't':
		// turn on defg
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		default:
		break;
	}
}
void display_7segments_LED_pitch(unsigned char fret, unsigned char str) {
	unsigned char tmp_letter_idx = strings_tuning[str - 1].letter_idx + fret;
	if(tmp_letter_idx > 11) {
		tmp_letter_idx %= 12;
	}
	
	// initialize Sharp LED
	SHARP_LED_OFF();
	
	switch(tmp_letter_idx) {
		case 0: // C
		display_7segments_char('C');
		break;
		
		case 1: // C#
		display_7segments_char('C');
		SHARP_LED_ON();
		break;
		
		case 2: // D
		display_7segments_char('d');
		break;
		
		case 3: // D#
		display_7segments_char('d');
		SHARP_LED_ON();
		break;
		
		case 4: // E
		display_7segments_char('E');
		break;
		
		case 5: // F
		display_7segments_char('F');
		break;
		
		case 6: // F#
		display_7segments_char('F');
		SHARP_LED_ON();
		break;
		
		case 7: // G
		display_7segments_char('G');
		break;
		
		case 8: // G#
		display_7segments_char('G');
		SHARP_LED_ON();
		break;
		
		case 9: // A
		display_7segments_char('A');
		break;
		
		case 10: // A#
		display_7segments_char('A');
		SHARP_LED_ON();
		break;
		
		case 11: // B
		display_7segments_char('b');
		break;
		
		default:
		break;
	}
}
enum DisplayAndPlayStates { DAP_init, DAP_idle, DAP_play};
int TickFct_DisplayAndPlay (int state) {
	switch(state) { // Transitions
		case DAP_init:
		state = DAP_idle;
		break;
		
		case DAP_idle:
		state = pressed_str_num > 0 ? DAP_play : DAP_idle;
		break;
		
		case DAP_play:
		state = pressed_str_num > 0 ? DAP_play : DAP_idle;
		break;
		
		default:
		state = DAP_init;
	}
	
	switch(state) { // Actions
		case DAP_init:
		break;
		
		case DAP_idle:
		display_7segments_char(0);
		SHARP_LED_OFF();
		PWM_off();
		break;
		
		case DAP_play:
		PWM_on();
		display_7segments_LED_pitch(pressed_fret_num, pressed_str_num);
		play_note(pressed_fret_num, pressed_str_num);
		break;
	}
	return state;
};