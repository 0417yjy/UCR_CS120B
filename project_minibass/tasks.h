/*
 * tasks.h
 * Description: Source of State Machines
 * Created: 2019-11-18 14:19:00
 *  Author: Jongyeon Yoon
 */ 
#include "declarations.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Task {
	int state; // Task's current state
	unsigned long period; // Task period
	unsigned long long elapsedTime; // Time elapsed since last task tick
	int (*TickFct) (int); // Task tick function
} Task;

typedef enum bool {false, true} bool;

#define TASK_SIZE 7
Task tasks[TASK_SIZE];

// shared variables
bool sv_mod = 0;
unsigned char sv_fn = 0;
unsigned char pressed_fret_num;
unsigned char pressed_str_num;
char sv_bpm_buffer[4] = "000";
unsigned short sv_metro_bpm;

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
		if(cnt > 2) {
			state = SM_programming_mode;
			sv_mod = PROG;
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
		state = (sv_mod == PROG) ? SM_programming_mode : SM_playing_mode;
		break;
		
		default:
		state = SM_init;
	}
	
	switch(state) { // Actions
		case SM_init:
		break;
		
		case SM_playing_mode:
		sv_mod = FP;
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
		if(sv_mod == FP) {
			if(is_str_pressed()) {
				state = BI_playing;
			}
			else if(is_fret_pressed()) {
				state = BI_fingering;
			}
			else {
				state = BI_idle;
			}
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

#define PRESCALER 64
void PWM_on() {
	TCCR3A = (1 << COM3A0); // CTC mode
	//TCCR3A = (1 << COM3A0) | (1 << WGM31) | (1 << WGM30); // Fast PWM Mode
	// COM3A: Toggle PB3 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30); // CTC
	//TCCR3B = (1 << WGM33) | (1 << WGM32) | (1 << CS31) | (1 << CS30); // Fast PWM mode
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	//set_PWM(0);
}
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		//if(!frequency) {TCCR3B &= 0x00;}
		if(!frequency) {TCCR3B &= 0x08;} // stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter (prescaler of 64)
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if(frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR3A from underflowing, using prescaler 64
		// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short) (8000000 / (2 * PRESCALER * frequency)) - 1; }
		
		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
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
	
	if(tmp_letter_idx > 11) { // calculate octave and pitch
		tmp_octave = tmp_octave + (tmp_letter_idx / 12);
		tmp_letter_idx %= 12;
	}
	
	// desired_frequency = (double) tmp_octave * pitches_octave1[tmp_letter_idx];
	desired_frequency = pow(2, tmp_octave) * pitches_octave1[tmp_letter_idx];
	set_PWM(desired_frequency);
}
void display_7segments_char(char ch) {
	// initialize 7-segments display
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
		SEVEN_DOT_ON();
		break;
		
		case 'P':
		// turn on abefg
		SEVEN_A_ON();
		SEVEN_B_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		case 't':
		// turn on defg
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		case '0':
		// turn on abcdef
		SEVEN_A_ON();
		SEVEN_B_ON();
		SEVEN_C_ON();
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		break;
		
		case '1':
		// turn on bc
		SEVEN_B_ON();
		SEVEN_C_ON();
		break;
		
		case '2':
		// turn on abdeg
		SEVEN_A_ON();
		SEVEN_B_ON();
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_G_ON();
		break;
		
		case '3':
		// turn on abcdg
		SEVEN_A_ON();
		SEVEN_B_ON();
		SEVEN_C_ON();
		SEVEN_D_ON();
		SEVEN_G_ON();
		break;
		
		case '4':
		// turn on bcfg
		SEVEN_B_ON();
		SEVEN_C_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		case '5':
		// turn on acdfg
		SEVEN_A_ON();
		SEVEN_C_ON();
		SEVEN_D_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		case '6':
		// turn on acdefg
		SEVEN_A_ON();
		SEVEN_C_ON();
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		case '7':
		// turn on abc
		SEVEN_A_ON();
		SEVEN_B_ON();
		SEVEN_C_ON();
		break;
		
		case '8':
		// turn on abcdefg
		SEVEN_A_ON();
		SEVEN_B_ON();
		SEVEN_C_ON();
		SEVEN_D_ON();
		SEVEN_E_ON();
		SEVEN_F_ON();
		SEVEN_G_ON();
		break;
		
		case '9':
		// turn on abcdfg
		SEVEN_A_ON();
		SEVEN_B_ON();
		SEVEN_C_ON();
		SEVEN_D_ON();
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
		if(sv_mod == FP) {
			display_7segments_char(0);
			SHARP_LED_OFF();
			PWM_off();
		}
		//set_PWM(0);
		break;
		
		case DAP_play:
		PWM_on();
		display_7segments_LED_pitch(pressed_fret_num, pressed_str_num);
		play_note(pressed_fret_num, pressed_str_num);
		break;
	}
	return state;
};

enum SelectFunctionStates { SF_init, SF_disabled, SF_select_fn};
int TickFct_SelectFunction (int state) {
	switch(state) { // Transitions
		case SF_init:
		state = SF_disabled;
		break;
		
		case SF_disabled:
		if(sv_mod == FP) {
			state = SF_disabled;
		}
		else if(sv_mod == PROG) {
			state = SF_select_fn;
		}
		break;
		
		case SF_select_fn:
		if(FRET_15) {
			state = SF_select_fn;
			sv_fn = FN_TUNING;
		}
		else if(FRET_14) {
			state = SF_select_fn;
			sv_fn = FN_METRO;
		}
		else if(sv_fn == FN_NOTSELECTED && FRET_11) {
			state = SF_disabled;
			sv_mod = FP;
		}
		else {
			state = SF_select_fn;
		}
		break;
		
		default:
		state = SF_init;
		break;
	}
	
	switch(state) { // Actions
		case SF_init:
		break;
		
		case SF_disabled:
		break;
		
		case SF_select_fn:
		SHARP_LED_OFF();
		switch(sv_fn) {
			case FN_NOTSELECTED:
			display_7segments_char('P');
			break;
			
			case FN_METRO:
			display_7segments_char('O');
			break;
			
			case FN_TUNING:
			display_7segments_char('t');
			break;
			
			default:
			break;
		}
		break;
	}
	return state;
};

void insert_bpm_buffer(unsigned char input) {
	if(input >= 0 && input <= 9) {
		sv_bpm_buffer[0] = sv_bpm_buffer[1]; // shift buffer to left once
		sv_bpm_buffer[1] = sv_bpm_buffer[2];
		sv_bpm_buffer[2] = '0' + input;
	}
}
enum metrOnomeStates { M_init, M_wait, M_user_input, M_user_input_pressed, M_save_bpm };
int TickFct_metrOnome (int state) {
	static unsigned char input = '0';
	static bool is_input_triggered = false;
	
	switch(state) { // Transitions
		case M_init:
		state = M_wait;
		break;
		
		case M_wait:
		if(sv_fn == FN_METRO) {
			state = M_user_input;
		}
		else {
			state = M_wait;
		}
		break;
		
		case M_user_input:
		if(is_fret_pressed()) {
			if(FRET_11) {
				// save inserted bpm
				state = M_save_bpm;
			}
			else if(FRET_15) {
				// change to tuning mode
				state = M_wait;
			}
			else if(FRET_14) {
				// re-enter metronome mode: initialize buffer
				strcpy(sv_bpm_buffer, "000");
				is_input_triggered = false;
				state = M_user_input;
			}
			else if(FRET_12 || FRET_13 || FRET_16 || FRET_17) {
				// invalid inputs: do nothing
				state = M_user_input;
			}
			else {
				// FRET1 ~ FRET10 pressed
				is_input_triggered = true;
				state = M_user_input_pressed;
				input = get_fret_num();
				if(input == 10) {
					input = 0;
				}
				insert_bpm_buffer(input);
				input += '0';
			}
		}
		else if(sv_fn != FN_METRO) {
			state = M_wait;
		}
		else {
			state = M_user_input;
		}
		break;
		
		case M_user_input_pressed:
		state = is_fret_pressed() ? M_user_input_pressed : M_user_input;
		break;
		
		case M_save_bpm:
		if(FRET_11) { // holding
			state = M_save_bpm;
		}
		else { // releasing
			state = M_wait;
			
			// initialize flag and buffer
			is_input_triggered = false;
			strcpy(sv_bpm_buffer, "000");
			
			// exit metronome mode
			sv_fn = FN_NOTSELECTED;
		}
		break;
		
		default:
		state = M_init;
		break;
	}
	
	switch(state) { // Actions
		case M_init:
		break;
		
		case M_wait:
		break;
		
		case M_user_input:
		if(is_input_triggered) {
			display_7segments_char(input);
		}
		break;
		
		case M_user_input_pressed:
		display_7segments_char(input);
		break;
		
		case M_save_bpm:
		display_7segments_char(input);
		sv_metro_bpm = (short) atoi(sv_bpm_buffer);
		if(sv_metro_bpm > 0) {
			tasks[5].period = (long) (1000 / ((float) sv_metro_bpm / 60)); // change period of BlinkLED SM
		}
		break;
	}
	return state;
};

enum BlinkLEDStates { BL_init, BL_led_off, BL_led_on};
int TickFct_BlinkLED (int state) {
	switch(state) { // Transitions
		case BL_init:
		state = BL_led_off;
		break;
		
		case BL_led_off:
		if(sv_metro_bpm > 0) {
			state = BL_led_on;
		}
		else {
			state = BL_led_off;
		}
		break;
		
		case BL_led_on:
		state = BL_led_off;
		break;
		
		default:
		state = BL_init;
		break;
	}
	
	switch(state) { // Actions
		case BL_init:
		break;
		
		case BL_led_off:
		METRO_LED_OFF();
		break;
		
		case BL_led_on:
		METRO_LED_ON();
		break;
	}
	return state;
};

void increment_tuning(unsigned char str) {
	if(strings_tuning[str - 1].letter_idx < 11) {
		strings_tuning[str - 1].letter_idx++;
	}
	else {
		strings_tuning[str - 1].letter_idx = 0;
		strings_tuning[str - 1].octave++;
	}
}
void decrement_tuning(unsigned char str) {
	if(strings_tuning[str - 1].letter_idx > 0) {
		strings_tuning[str - 1].letter_idx--;
	}
	else {
		if(strings_tuning[str - 1].octave > 0) {
			strings_tuning[str - 1].letter_idx = 11;
			strings_tuning[str - 1].octave--;
		}
	}
}
void reset_tuning(unsigned char str) {
	switch (str) {
		case 1:
		strings_tuning[0].octave = 0;
		strings_tuning[0].letter_idx = 4;
		break;
		
		case 2:
		strings_tuning[1].octave = 0;
		strings_tuning[1].letter_idx = 9;
		break;
		
		case 3:
		strings_tuning[1].octave = 1;
		strings_tuning[1].letter_idx = 2;
		break;
		
		case 4:
		strings_tuning[1].octave = 1;
		strings_tuning[1].letter_idx = 7;
		break;
		
		default:
		break;
	}
}
enum TuningStates { T_init, T_wait, T_select_str, T_reset_tune, T_adjust_tune, T_save_quit };
int TickFct_Tuning (int state) {
	static unsigned char selected_str = 0;
	static bool is_input_triggered = false;
	
	switch(state) { // Transitions
		case T_init:
		state = T_wait;
		break;
		
		case T_wait:
		state = (sv_fn == FN_TUNING) ? T_select_str : T_wait;
		break;
		
		case T_select_str:
		if(sv_fn != FN_TUNING) {
			state = T_wait;
		}
		else if(is_str_pressed()) {
			state = T_select_str;
			selected_str = get_str_num(selected_str);
			is_input_triggered = true;
		}
		else if(FRET_11) {
			state = T_save_quit;
		}
		else if(is_input_triggered && FRET_12 && FRET_13) {
			state = T_reset_tune;
		}
		else if(is_input_triggered && (FRET_12 || FRET_13)) {
			state = T_adjust_tune;
			if(FRET_12) {
				increment_tuning(selected_str);
			}
			else if(FRET_13) {
				decrement_tuning(selected_str);
			}
		}
		else {
			state = T_select_str;
		}
		break;
		
		case T_reset_tune:
		if(!FRET_12 && !FRET_13) {
			state = selected_str;
		}
		else {
			state = T_reset_tune;
		}
		break;
		
		case T_adjust_tune:
		if(FRET_12 && FRET_13) {
			state = T_reset_tune;
		}
		else if(!FRET_12 && !FRET_13) {
			state = T_select_str;
		}
		else if(FRET_12 || FRET_13) {
			state = T_adjust_tune;
		}
		break;
		
		case T_save_quit:
		if(FRET_11) { // holding
			state = T_save_quit;
		}
		else { // releasing
			state = T_wait;
			selected_str = 0;
			sv_fn = FN_NOTSELECTED;
			is_input_triggered = false;
		}
		break;
		
		default:
		state = T_init;
		break;
	}
	
	switch(state) { // Actions
		case T_init:
		break;
		
		case T_wait:
		break;
		
		case T_select_str:
		if(is_input_triggered) {
			display_7segments_LED_pitch(0, selected_str);
		}
		break;
		
		case T_reset_tune:
		reset_tuning(selected_str);
		display_7segments_LED_pitch(0, selected_str);
		break;
		
		case T_adjust_tune:
		display_7segments_LED_pitch(0, selected_str);
		break;
		
		case T_save_quit:
		display_7segments_LED_pitch(0, selected_str);
		break;
	}
	return state;
};