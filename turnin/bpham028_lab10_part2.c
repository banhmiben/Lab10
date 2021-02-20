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

unsigned char key = 0x00;
unsigned char tmp = 0x00;
//unsigned char led = 0x00;
//unsigned char cnt = 0x00;

enum UnlockStates {Init1, Pound, WaitP, One, Wait1, Two, Wait2, Three, Wait3, Four, Wait4, Five, Wait5, Unlock1};
int UnlockSMTick(int state) {

	key = GetKeypadKey();
	switch(state) {
		case(Init1):
		/*	if(key == '#') {
				PORTA = 0x0F;
			} else if (key == '1') {
				PORTA = 0x01;
			} else if (key == '2') {
				PORTA = 0x02;
			} else if (key == '3') {
				PORTA = 0x03;
			} else if (key == '4') {
				PORTA = 0x04;
			} else if (key == '5') {
				PORTA = 0x05;
			} else if (key == '\0') {
				PORTA = 0x07;
			} break;*/
			if (key == '#') {
				state = Pound;
			} else {
				state = Init1;
			} break;
		case(Pound):
			if (key == '\0') {
				state = WaitP;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(WaitP):
			if (key == '1') {
				state = One;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(One):
			if (key == '\0') {
				state = Wait1;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(Wait1):
			if (key == '2') {
				state = Two;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(Two):
			if (key == '\0') {
				state = Wait2;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(Wait2):
			if (key == '3') {
				state = Three;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(Three):
			if (key == '\0') {
				state = Wait3;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(Wait3):
			if (key == '4') {
				state = Four;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(Four):
			if (key == '\0') {
				state = Wait4;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(Wait4):
			if (key == '5') {
				state = Five;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(Five):
			if (key == '\0') {
				state = Unlock1;
			} else if (key == '#') {
				state = Pound;
			} break;
		case(Unlock1):
			if (key == '\0') {
				state = Unlock1;
			}
		        if ((~PINB & 0x80) == 0x80) {
				state = Init1;
			} break;
		default:
			state = Init1;
			break;

	}
	switch(state) {
		case(Init1): PORTB = 0x00; break;
		case(Pound): break;
		case(WaitP): break;
		case(One): break;
		case(Wait1): break;
		case(Two): break;
		case(Wait2): break;
		case(Three): break;
		case(Wait3): break;
		case(Four): break;
		case(Wait4): break;
		case(Five): break;
		case(Unlock1): PORTB = 0x01; break;
		default: break;
	}
	return state;

}

enum LockStates {Start, Init2, Unlock2};
int LockSMTick(int state) {

	switch(state) {
		case(Start):
			state = Init2;
			break;
		case(Init2):
			if ((~PINA & 0x01) == 0x01) {
				state = Unlock2;
			} else {
				state = Init2;
			} break;
		case(Unlock2):
			state = Init2;
			break;
		default:
			state = Start;
			break;
	}
	switch(state) {
		case(Start): break;
		case(Init2): break;
		case(Unlock2):
			PORTB = 0x00;
			break;
		default: break;
	}
	return state;

}

int main() {

	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0x7F; PORTB = 0x80; //output
	DDRC = 0xF0; PORTC = 0x0F; //input

	static task task1, task2;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;
	//Task1
	task1.state = Init1;
	task1.period = 10;
	task1.elapsedTime = task1.period;
	task1.TickFct = &UnlockSMTick;

	task2.state = start;
	task2.period = 10;
	task2.elapsedTime = task2.period;
	task2.TickFct = &LockSMTick;

	TimerSet(10);
	TimerOn();

	unsigned short i; //scheduler loop iterator
	while(1) {
		for (i = 0; i < numTasks; i++) {
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
