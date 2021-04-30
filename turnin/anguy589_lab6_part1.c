/*	Author: anguy589
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *      Demo Link: https://drive.google.com/file/d/1cHZbc9BPBHUyuy9kgIm_PRzJfL7z0ZVE/view?usp=sharing
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//timer.h
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1; //start count, down to 0. default 1 ms
unsigned long _avr_timer_cntcurr = 0;// current interal count of 1 ms ticks

enum states {smstart, init, light1, light2, light3} state;
unsigned char tempC = 0x00;

void TimerOn() {

	TCCR1B = 0x0B; //AVR timer/counter controller register
	OCR1A = 125; // AVR output compare register
	TIMSK1 = 0x02; // AVR timer interrupt mask register

	TCNT1 = 0; //initialize AVR counter

         //TimerISR will be called every _avr_timer_cntcurr milliseconds
	_avr_timer_cntcurr = _avr_timer_M; 
	
	SREG |= 0x80;//enable global interrupts
}

void TimerOff() {
	TCCR1B = 0x00; //timer off
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	
	}
}

//set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


//SynchSM

void lightTick() {
	//state transitions
	switch(state) {
		case smstart:
			state = init;
			break;
		case init:
			state = light1;
			break;
		case light1:
			state = light2;
			break;
		case light2:
			state = light3;
			break;
		case light3:
			state = light1;
			break;
		default:
			state = init;
			break;
	}

	//state actions
	switch(state) {
		case smstart:
			break;
		case init:
			tempC = 0x00;
			break;
		case light1:
			tempC = 0x01;
			break;
		case light2:
			tempC = 0x02;
			break;
		case light3:
			tempC = 0x04;
			break;
		default:
			break;

	}



}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00;

	TimerSet(1000);
	TimerOn();

	state = smstart;
    
    while (1) {
	lightTick();

    	while(!TimerFlag); 
	TimerFlag = 0;

	PORTC = tempC;
		    
    }
    return 1;
}
