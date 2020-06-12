/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab # 7  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

unsigned char count = 0;
unsigned char hold = 0;
enum States{Start, store, incr, incrHold, decr, decrHold, reset} state;
void Tick(){
	unsigned char tempA = ~PINA;
	//transitions
	switch(state){
		case Start:
			state = store;
			break;
		case store:
			if((tempA & 0x03) ==  0x01){
				state = incr;
			}
			else if((tempA & 0x03) == 0x02){
				state = decr;
			}
			else if((tempA & 0x03) == 0x03){
				state = reset;
			}
			else{
				state = store;
			}
			break;
		case incr:
			if((tempA & 0x03) == 0x01){
				state = incrHold;
			}
			else if((tempA & 0x03) == 0x02){
				state = decr;
			}
			else if((tempA & 0x03) == 0x03){
				state = reset;
			}
			else{
				state = store;
			}
			break;
		case incrHold:
			if((tempA & 0x03) == 0x01 && hold < 10){
				state = incrHold;
			}
			else if((tempA & 0x03) == 0x01 && hold == 10){
				hold = 0;
				state = incr;
			}
			else if((tempA & 0x03) == 0x02){
				hold = 0;
				state = decr;
			}
			else if((tempA & 0x03) == 0x03){
				hold = 0;
				state = reset;
			}
			else{
				hold = 0;
				state = store;
			}
			break;
		case decr:
			if((tempA & 0x03) == 0x01){
				state = incr;
			}
			else if((tempA & 0x03) == 0x02){
				state = decrHold;
			}
			else if((tempA & 0x03) == 0x03){
				state = reset;
			}
			else{
				state = store;
			}
			break;
		case decrHold:
			if((tempA & 0x03) == 0x02 && hold < 10){
				state = decrHold;
			}
			else if((tempA & 0x03) == 0x02 && hold == 10){
				hold = 0;
				state = decr;
			}
			else if((tempA & 0x03) == 0x01){
				hold = 0;
				state = incr;
			}
			else if((tempA & 0x03) == 0x03){
				hold = 0;
				state = reset;
			}
			else{
				hold = 0;
				state = store;
			}

			break;
		case reset:
			if((tempA & 0x03) == 0x03){
				state = reset;
			}
			else if((tempA & 0x03) == 0x01){
				state = incr;
			}
			else if((tempA & 0x03) == 0x02){
				state = decr;
			}
			else{
				state = store;
			}
			break;
	
		default:
			state = Start;
			break;
	}
	//state actions
	switch(state){
		case Start:
			break;
		case store:
			LCD_ClearScreen();
		 	LCD_WriteData(count + '0');
			break;
		case incr:
			if(count != 9){
				count++;
			}
			LCD_ClearScreen();
			LCD_WriteData(count + '0');
			break;
		case incrHold:
			hold++;
			break;
		case decr:
			if(count != 0){
				count--;
			}
			LCD_ClearScreen();
			LCD_WriteData(count + '0');
			break;
		case decrHold:
			hold++;
			break;
		case reset:
			count = 0;
			LCD_ClearScreen();
			LCD_WriteData(count + '0');
			break;
	
		default:
			break;

	}

}
void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	
	DDRC = 0xff;	PORTC = 0x00;
	DDRD = 0xff;	PORTD = 0x00;
	DDRA = 0x00;	PORTA = 0xff;

	LCD_init();
	
       	TimerSet(100);
	TimerOn();	
	
    /* Insert your solution below */
    while (1) {
	Tick();
	while(!TimerFlag);

	TimerFlag = 0;
    }
    return 1;
}
