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
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

unsigned int tempVal = 1;
unsigned int count = 5;
enum States{start, l0, l1, l2, hold, release} state;

void Tick(){
	unsigned char tempA = ~PINA;
	switch(state){
		case start:
			state = l0;
			LCD_WriteData(count + '0');
			break;
		case l0:
			if((tempA & 0x01) == 0x01){
				state = hold;
				if(count != 0){
					count--;
				}
			}
			else{
				state = l1;
			}
			break;
		case l1:
			if((tempA & 0x01) == 0x01){
				state = hold;
				if(count != 9){
					count++;
				}
			}
			else if(tempVal == 1){
				state = l2;
			}
			else{
				state = l0;
			}
			break;
		case l2:
			if((tempA & 0x01) == 0x01){
				state = hold;
				if(count != 0){
					count--;
				}
			}
			else{
				state =l1;
			}
			break;
		case hold:
			if((tempA & 0x01) == 0x01){
				state = hold;
			}
			else{
				state = release;
			}
			break;
		case release:
			if((tempA & 0x01) == 0x01){
				LCD_ClearScreen();
				LCD_WriteData(count + '0');
				state = l0;
			}
			else{
				state = release;
			}
			break;

		default:
			state = start;
			break;
	}

	switch(state){
		case start:
			break;
		case l0:
			tempVal = 1;
			PORTB = (PORTB | 0x01) & 0x01;
			break;
		case l1:
			PORTB = (PORTB | 0x02) & 0x02;
			break;
		case l2:
			tempVal = 2;
			PORTB = (PORTB | 0x04) & 0x04;
			break;
		case hold:
			LCD_ClearScreen();
			if(count != 9){
				LCD_WriteData(count + '0');
			}
			else{
				LCD_DisplayString(1, "Winner");
				count = 5;
			}
			break;
		case release:
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
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00

    LCD_init();	    
    TimerSet(300);
    TimerOn();

    /* Insert your solution below */
    while (1) {
	Tick();
	while(!TimerFlag);

	TimerFlag = 0;
    }
    return 1;
}
