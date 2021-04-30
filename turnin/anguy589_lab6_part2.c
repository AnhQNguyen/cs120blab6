/*	Author: anguy589
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 * 	
 * 	Demo Link: https://drive.google.com/file/d/1-eHAPXJtB02-JopkZDKJjAqqtDMX5JOC/view?usp=sharing
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

enum states {init, light1, light2, light3, wait, hold1, hold2, hold3} state;
unsigned char tempC = 0x00;
unsigned char button = 0x00;
unsigned char currState;
unsigned char flag;

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
	button = ~PINA & 0x01;

	//state transitions
	switch(state) {
		case init:
			state = light1;
			break;
		case light1:
			if(button) {
				state = wait;
			}
			else {
				state = light2;
			}
			break;
		case light2:
			if(button) {
				state = wait;
			}
			else if(flag) {
				state = light1;
			}
			else {
				state = light3;
			}
			break;
		case light3:
			if(button) {
				state = wait;
			}
			else {
				state = light2;
			}
			break;
		case wait:
			switch(currState) {
				case 1:	
					state = hold1;
					break;
				case 2:
					state = hold2;
					break;
				case 3:
					state = hold3;
					break;
				default:
					break;
			}
			break;
		case hold1:
			if(button) {
				state = init;
			}
			else {
				state = hold1;
			}
			break;
		case hold2:
			if(button) {
                                state = init;
                        }
                        else {
                                state = hold2;
                        }
                        break;
		case hold3:
                        if(button) {
                                state = init;
                        }
                        else {
                                state = hold3;
                        }
                        break;
			
		default:
			state = init;
			break;
	}

	//state actions
	switch(state) {
		case init:
			tempC = 0x00;
			flag = 0;
			currState = 0;
			break;
		case light1:
			currState = 1;
			tempC = 0x01;
			flag = 0;
			break;
		case light2:
			currState = 2;
			tempC = 0x02;
			break;
		case light3:
			currState = 3;
			tempC = 0x04;
			flag = 1;
			break;
		case wait:
			break;
		case hold1:
			tempC = 0x01;
			break;
		case hold2:
			tempC = 0x02;
			break;
		case hold3:
			tempC = 0x04;
			break;
		default:
			break;

	}



}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00;
	DDRA = 0x00; PORTA = 0xFF;


	TimerSet(300);
	TimerOn();

	state = init;
    
    while (1) {
	lightTick();

    	while(!TimerFlag); 
	TimerFlag = 0;

	PORTC = tempC;
		    
    }
    return 1;
}
