#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "include/uart.h"

#define BAUD	9600

short menuIsActive = 1;
short timerStarted = 0;
short configMenuIsActive = 0;

/*
 *	Time measures are in seconds
 */
typedef config {
	short savePower = 1;
	unsigned int long_period;
	unsigned int short_period;
	unsigned char repetitions;
}config;


void printMenu()
{
	usart_puts("AVR-POMODORO v0.1\n\r\tMENU\n\r1 - Start timer\n\r2 - Configure timer\n\n");
}


void set_timer (unsigned int time_in_seconds)
{
	/* Calculate timer ticks */
	

	/* Calculate overflows */
	
}

/* This es the OK button */
ISR(INT0_vect)
{

}

/* This is the SELECT button */
ISR(INT1_vect)
{

}

int main(){

	cli();

	/* Configure input and set pullups for them */
	DDRB &= ~((1<<PORTD3)|(1<<PORTD2));
	PORTB |= (1<<PORTD3)|(1<<PORTD2);

	/* Configure them as interrupts and select mode of operation */
	EIMSK |= (1<<INT0)|(1<<INT1);
	EICRA = 0x00;

	/* Preconfig timer 1 */
	TCCR1A = 0x00;
	

	uart_init(UART_BAUD_SELECT(BAUD,F_CPU));
	
	sei();
}
