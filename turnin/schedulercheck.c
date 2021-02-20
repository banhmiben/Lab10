/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <timer.h>
#include <keypad.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

// shared variables
unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;

enum pauseButtonSM_States { pauseButton_wait, pauseButton_press, pauseButton_release };
//monitors buttons connected to PA0
//When button is pressed, shared vairable "pause" is toggled
int pauseButtonToggleSMTick(int state) {

	unsigned char press = ~PINA & 0x01;

	switch(state) {//transitions
		case(pauseButton_wait):
			state = press == 0x01 ? pauseButton_press : pauseButton_wait; break;
		case(pauseButton_press):
			state = pauseButton_release; break;
		case(pauseButton_release):
			state = press == 0x00 ? pauseButton_wait : pauseButton_press; break;
		default: state = pauseButton_wait; break;
	}
	switch(state) {//actions
		case(pauseButton_wait): break;
		case(pauseButton_press):
			pause = (pause == 0) ? 1 : 0; //toggle pause
			break;
		case(pauseButton_release): break;
	}
	return state;

}

enum toggleLED0_States { toggleLED0_wait, toggleLED0_blink };
//if paused: Do NOT toggle LED on PB0
//if unpaused: toggle LED PB0
int toggleLED0SMTick(int state) {
	switch (state) { //transitions
		case toggleLED0_wait: state = !pause ? toggleLED0_blink: toggleLED0_wait; break;
		case toggleLED0_blink: state = pause ? toggleLED0_wait: toggleLED0_blink; break;
		default: state = toggleLED0_wait; break;
	}
	switch (state) {
		case toggleLED0_wait: break;
		case toggleLED0_blink:
			led0_output = (led0_output == 0x00) ? 0x01 : 0x00; //toggle LED
			break;
	}
	return state;
}

enum toggleLED1_States { toggleLED1_wait, toggleLED1_blink };
//paused: no PB1 toggle
//unpaused: toggle PB1
int toggleLED1SMTick(int state) {
	switch (state) { //transitions
		case toggleLED1_wait: state = !pause ? toggleLED1_blink: toggleLED1_wait; break;
		case toggleLED1_blink: state = pause ? toggleLED1_wait: toggleLED1_blink; break;
		default: state = toggleLED1_wait; break;
	}
	switch (state) {
		case toggleLED1_wait: break;
		case toggleLED1_blink:
			led1_output = (led1_output == 0x00) ? 0x01 : 0x00; //toggle LED
			break;
	}
	return state;
}

enum display_States { display_display };
//combine blinking LED outputs from toggleLED0 SM and toggleLED1 SM and output on PORTB
int displaySMTick(int state) {
	unsigned char output;
	switch(state) {
		case display_display: state = display_display; break;
		default: state = display_display; break;
	}
	switch(state) {
		case(display_display):
			output = led0_output | led1_output << 1;
			break;
	}
	PORTB = output;
	return state;
}

int main() {

	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	//declare array of tasks
	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;
	//Task 1
	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &pauseButtonToggleSMTick;
	//Task 2
	task2.state = start;
	task2.period = 500;
	task2.elapsedTime = task2.period;
	task2.TickFct = &toggleLED0SMTick;
	//Task 3
	task3.state = start;
	task3.period = 1000;
	task3.elapsedTime = task3.period;
	task3.TickFct = &toggleLED1SMTick;
	//Task 4
	task4.state = start;
	task4.period = 10;
	task4.elapsedTime = task4.period;
	task4.TickFct = &displaySMTick;

	//Set timer and turn on
	TimerSet(10);
	TimerOn();

	unsigned short i; //Scheduler for-loop iterator
	while(1) {
		for (i =0; i < numTasks; i++) {
			if (tasks[i] -> elapsedTime == tasks[i] -> period) {
				tasks[i] -> state = tasks[i] -> TickFct(tasks[i] -> state);
				tasks[i] -> elapsedTime = 0;
			}
			tasks[i] -> elapsedTime += 10;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}
