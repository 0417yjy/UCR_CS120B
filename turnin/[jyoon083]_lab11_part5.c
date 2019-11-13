/*	Author: Jongyeon Yoon
 *  Partner(s) Name: Weifeng
 *	Lab Section:021
 *	Assignment: Lab #11  Exercise #5
 *	Exercise Description: 
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
	int state; // Task's current state
	unsigned long period; // Task period
	unsigned long long elapsedTime; // Time elapsed since last task tick
	int (*TickFct) (int); // Task tick function
} Task;

#define TASK_SIZE 3
Task tasks[TASK_SIZE];

typedef enum boolean {false, true} boolean;
// shared variables
unsigned char player_position;
boolean game_operating;
boolean game_paused;

enum TurnOnOffStates {TOO_init, TOO_off, TOO_on_press, TOO_on_release, TOO_pause_press, TOO_pause_release};
int TickFct_TurnOnOffStates(int state) {
	unsigned char A0 = ~PINA & 0x01;
	
	switch(state) { // Transitions
		case TOO_init:
			state = TOO_off;
			game_operating = false;
			break;
		case TOO_off:
			if(A0) {
				state = TOO_on_press;
				game_operating = true;
			}
			else {
				state = TOO_off;
			}
			break;
		case TOO_on_press:
			if(!game_operating) {
				state = TOO_off;
			}
			else {
				state = A0 ? TOO_on_press : TOO_on_release;
			}
			break;
		case TOO_on_release:
			if(!game_operating) {
				state = TOO_off;
			}
			else if(A0) {
				state = TOO_pause_press;
			}
			else {
				state = TOO_on_release;
			}
			break;
		case TOO_pause_press:
			state = A0 ? TOO_pause_press : TOO_pause_release;
			break;
		case TOO_pause_release:
			if(A0) {
				state = TOO_on_press;
			}
			else {
				state = TOO_pause_release;
			}
		default:
			state = TOO_init;
			break;
	}
	switch(state) { // Actions
		case TOO_init:
			break;
		case TOO_off:
			break;
		case TOO_on_press:
			game_paused = false;
			break;
		case TOO_on_release:
			break;	
		case TOO_pause_press:
			game_paused = true;
			break;
		case TOO_pause_release:
			break;
	}
	return state;	
};

#define TOP_ROW 2
#define BOT_ROW 18
enum PlayerMoveStates {PM_init, PM_wait, PM_top, PM_bot, PM_both};
int TickFct_PlayerMove(int state) {
	unsigned char A1 = ~PINA & 0x02; // Move to bottom row
	unsigned char A2 = ~PINA & 0x04; // Move to top row
	
	switch(state) { // Transitions
		case PM_init:
			player_position = TOP_ROW;
			LCD_Cursor(player_position);
			state = PM_wait;
			break;
		case PM_wait:
			if(A2) {
				player_position = TOP_ROW;
				LCD_Cursor(player_position);
				state = PM_top;
			}
			else if(A1) {
				player_position = BOT_ROW;
				LCD_Cursor(player_position);
				state = PM_bot;
			}
			else {
				state = PM_wait;
			}
			break;
		case PM_top:
			if(A1) {
				player_position = BOT_ROW;
				LCD_Cursor(player_position);
				state = PM_both;
			}
			else if(A2) {
				state = PM_top;
			}
			else {
				state = PM_wait;
			}
			break;
		case PM_bot:
			if(A2) {
				player_position = TOP_ROW;
				LCD_Cursor(player_position);
				state = PM_both;
			}
			else if(A1) {
				state = PM_top;
			}
			else {
				state = PM_wait;
			}
			break;
		case PM_both:
			if(A1 || A2) {
				state = PM_both;
			}
			else {
				state = PM_wait;
			}
			break;
		default:
			state = PM_init;
	}
	switch(state) { // Actions
		case PM_init:
			break;
		case PM_wait:
			break;
		case PM_top:
			break;
		case PM_bot:
			break;
		case PM_both:
			break;
	}
	return state;
};

typedef enum obstacle_type {dead, speedup, speeddown} obstacle_type;
typedef struct Obstacle {
	boolean is_top_row; // if true, obstacle is in top row
	unsigned char position; // current position of this obstacle
	obstacle_type type;
} Obstacle;

#define MAX_OBS 8
Obstacle obs_arr[MAX_OBS];
unsigned char obs_num = 0;


enum GameManageStates {GM_init, GM_over, GM_work};
void generate_obstacle() {
	double rnd_type = (double) rand() / RAND_MAX;
	/* 60% dead
	 * 20% speedup
	 * 10% speeddown
	 * 10% doesn't occur
	*/
	double rnd_position = (double) rand() / RAND_MAX;
	// 50% top / 50% bottom
	
	// decide its type
	if(rnd_type < 0.1) {
		// do nothing
		return;
	}
	else if(rnd_type < 0.2) {
		obs_arr[obs_num].type = speeddown;
	}
	else if(rnd_type < 0.4) {
		obs_arr[obs_num].type = speedup;
	}
	else {
		obs_arr[obs_num].type = dead;
	}
	
	// decide its position
	if(rnd_position < 0.5) {
		// make obstacle in top row
		obs_arr[obs_num].is_top_row = true;
		obs_arr[obs_num].position = 17;
		obs_num++;
	}
	else {
		// make obstacle in bottom row
		obs_arr[obs_num].is_top_row = false;
		obs_arr[obs_num].position = 33;
		obs_num++;
	}
};
int TickFct_GameManage(int state) {
	static boolean collision_detected;
	static unsigned char obs_generate_cnt = 0;
	static unsigned long score = 0;
	char i, j; // used in for loop
	char score_str[32] = "Your score is:   ";
	char tmp_score[10];
	
	switch(state) { // Transitions
		case GM_init:
			state = GM_over;
			LCD_ClearScreen();
			LCD_DisplayString(4, "Hurray for   Embedded Systems");
			break;
		case GM_over:
			if(game_operating) {
				state = GM_work;
				generate_obstacle();
			}
			else {
				state = GM_over;
			}
			break;
		case GM_work:
			if(collision_detected) {
				state = GM_over;
				LCD_ClearScreen();
				sprintf(tmp_score, "%d", score);
				strcat(score_str, tmp_score);
				LCD_DisplayString(1, score_str);
				
				// resetting game
				obs_num = 0;
				obs_generate_cnt = 0;
				tasks[2].period = 500;
				game_operating = false;
				collision_detected = false;
				score = 0;
			}
			else {
				state = GM_work;
			}
			break;
		default:
			state = GM_init;
	}
	switch(state) { // Actions
		case GM_init:
			break;
		case GM_over:
			break;
		case GM_work:
			if(!game_paused) {
				// generate obstacle per 2 ticks
				obs_generate_cnt++;
				if(obs_generate_cnt > 2) {
					generate_obstacle();
					obs_generate_cnt = 0;
				}
				
				// if first obstacle is in leftmost position, delete it and fix the array
				if(obs_arr[0].is_top_row) {
					if(obs_arr[0].position <= 1) {
						if(obs_arr[0].type == dead) {
							score++;
						}
						for(i=1;i<obs_num;i++) {
							obs_arr[i - 1] = obs_arr[i];
						}
						obs_num--;
					}
				}
				else if(!obs_arr[0].is_top_row) {
					if(obs_arr[0].position <= 17) {
						if(obs_arr[0].type == dead) {
							score++;
						}
						for(i=1;i<obs_num;i++) {
							obs_arr[i - 1] = obs_arr[i];
						}
						obs_num--;
					}
				}
				
				// shift obstacles to left and display them in LCD
				LCD_ClearScreen();
				for(i=0;i<obs_num;i++) {
					obs_arr[i].position--;
					LCD_Cursor(obs_arr[i].position);
					switch(obs_arr[i].type) {
						case dead:
							LCD_WriteData('#');
							break;
						case speedup:
							LCD_WriteData('+');
							break;
						case speeddown:
							LCD_WriteData('-');
							break;
					}
				}
				
				// detect collision
				for(i=0;i<obs_num;i++) {
					if(obs_arr[i].position == player_position) {
						switch(obs_arr[i].type) {
							case dead:
								collision_detected = true;
								break;
							case speedup:
								if(tasks[2].period > 50) {
									tasks[2].period -= 50;
								}
								break;
							case speeddown:
								tasks[2].period += 50;
								break;
						}
						//delete collided obstacle
						for(j = i + 1;j<obs_num;j++) {
							obs_arr[j - 1] = obs_arr[j];
						}
					}
				}
				
				// display player cursor in LCD
				LCD_Cursor(player_position);
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
	DDRA = 0x00; PORTA = 0xFF; // Button inputs for PORTA
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// seed for random generator
	srand(time(NULL));
	
	// Initializes the LCD display
	LCD_init();
	
	unsigned char i = 0;
	tasks[i].state = TOO_init;
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_TurnOnOffStates;
	i++;
	tasks[i].state = PM_init;
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_PlayerMove;
	i++;
	tasks[i].state = GM_init;
	tasks[i].period = 500;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_GameManage;
	i++;
	
	TimerSet(1);
	TimerOn();
	
	while(1) { }
	
	return 0;
}