/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

/*
Modify the keypad code to be in an SM task and add an LED on PB7 then will light
up while any button is pressed, consider this immediate feedback to the user. Then,
modify the keypad SM to utilize the simple task scheduler format. All code from
here on out should use the task scheduler.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <timer.h>
#include <keypad.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char x = 0x00;

enum KeypadStates {Start, GetKey} ;
int KeypadSMTick(int state) {

	x = GetKeypadKey();
	switch(state) {//transitions
		case(Start):
			state = GetKey;
			break;
		case(GetKey):
			state = GetKey;
			break;
		default:
			state = Start;
			break;
	}
	switch(state) {
		case(Start): break;
		case(GetKey):
			switch(x) {
				case '\0': PORTB = 0x1F; break;
				case '1': PORTB = 0x81; break;
				case '2': PORTB = 0x82; break;
				case '3': PORTB = 0x83; break;
				case '4': PORTB = 0x84; break;
				case '5': PORTB = 0x85; break;
				case '6': PORTB = 0x86; break;
				case '7': PORTB = 0x87; break;
				case '8': PORTB = 0x88; break;
				case '9': PORTB = 0x89; break;
				case 'A': PORTB = 0x8A; break;
				case 'B': PORTB = 0x8B; break;
				case 'C': PORTB = 0x8C; break;
				case 'D': PORTB = 0x8D; break;
				case '*': PORTB = 0x8E; break;
				case '0': PORTB = 0x80; break;
				case '#': PORTB = 0x8F; break;
				default: PORTB = 0x1B; break; //should never occur
			}
		default: break;
	}
	return state;
}

int main() {

	DDRB = 0xFF; PORTB = 0x00; //output
	DDRC = 0xF0; PORTC = 0x0F; //input

	static task task1;
	task *tasks[] = {&task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;
	//Task1
	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &KeypadSMTick;

	TimerSet(50);
	TimerOn();

	unsigned short i; //scheduler loop iterator
	while(1) {
		for (i = 0; i < numTasks; i++) {
			if (tasks[i] -> elapsedTime == tasks[i] -> period) {
				tasks[i] -> state = tasks[i] -> TickFct(tasks[i] -> state);
				tasks[i] -> elapsedTime = 0;
			}
			tasks[i] -> elapsedTime += 50;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;

}
