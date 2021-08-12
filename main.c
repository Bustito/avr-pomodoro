#define F_CPU 16000000

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "include/uart.h"

#define BAUD	9600
#define UART_TX_BUFFER_SIZE 128

#define TIMER_FREQ	15625UL

short timer1IsDone = 0;

/********************************
 *	Time measures are in seconds
 ********************************/
unsigned long total_ticks = 0;
volatile unsigned long timer1_remaining_ticks = 0;
volatile unsigned char timer1_remaining_ovf = 0;

volatile unsigned int timer0_remaining_ticks = 0;
volatile unsigned char timer0_remaining_ovf = 0;

void usart_putc(unsigned char data)
{
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

int usart_putcf(char var, FILE *stream) {
    // translate \n to \r for br@y++ terminal
    if (var == '\n') usart_putc('\r');
    usart_putc(var);
    return 0;
}

void usart_puts(char *StringPtr)
{
	while(*StringPtr != 0x00)
	{  
	usart_putc(*StringPtr);    
	StringPtr++;
	} 
}

static FILE mystdout = FDEV_SETUP_STREAM(usart_putcf, NULL, _FDEV_SETUP_WRITE);

void printMenu()
{
	uart_puts("\n\rAVR-POMODORO v0.1\n\n\r\tMENU");
	uart_puts("\n\r1 - Sta");
	uart_puts("rt timer\n\r2 - Configure timer\n\n\r");
}

void init_timer0()
{
	/* Set timer 0 to normal work */
	TCCR0A = 0x00;

	/* Stop timer 0 */
	TCCR0B = 0x00;

	/* Set interrupts for overflow and ouput compare unit A */
	TIMSK0 = (1<<OCIE0A)|(1<<TOIE0);

}

/* This timer is for refreshing the display in counting mode */
void set_timer0 ()
{
	timer0_remaining_ticks = 70;
	timer0_remaining_ovf = 61;

	/* Set compare unit A to interrupt at the remaining ticks */
	OCR0A = timer0_remaining_ticks;

	/* Set timer 0 counter to 0 */
	TCNT0 = 0;

	/* Set timer 0 clock freq to F_CPU/1024 */
	TCCR0B |= (1<<CS02)|(1<<CS00);
}

void init_timer1()
{
	/* Set timer 1 to normal work */
	TCCR1A = 0x00;

	/* Set timer 1 clock freq to F_CPU/1024 */
	TCCR1B = (1<<CS12)|(1<<CS10);

	/* Set interrupts for overflow and ouput compare unit A */
	TIMSK1 = (1<<OCIE1A)|(1<<TOIE1);
}

void set_timer1 (unsigned int time_in_seconds)
{
	total_ticks = time_in_seconds * TIMER_FREQ;
	//total_ticks = 937500;
	/* Calculate overflows */
	timer1_remaining_ovf = (total_ticks / 65535);

	/* Calculate remaining ticks */
	timer1_remaining_ticks = total_ticks - (timer1_remaining_ovf * 65535);
	
	printf("Total ticks: %ld\nTotal overflows: %d\nRemaining ticks: %ld\n", total_ticks, timer1_remaining_ovf, timer1_remaining_ticks);

	/* Set timer 1 counter to 0 */
	TCNT1H = 0;
	TCNT1L = 0;

	/* Set compare unit A to interrupt at the remaining ticks */
	OCR1A = timer1_remaining_ticks;
}

void set_interrupts()
{
	/* Configure input and set pullups for them */
	DDRB &= ~((1<<PORTD3)|(1<<PORTD2));
	PORTB |= (1<<PORTD3)|(1<<PORTD2);

	/* Configure them as interrupts and select mode of operation */
	EIMSK |= (1<<INT0)|(1<<INT1);
	EICRA = 0x00;
}

int main(){
	cli();

	DDRB = (1<<DDB5);
	stdout = &mystdout;
	uart_init(UART_BAUD_SELECT(BAUD,F_CPU));

	init_timer0();
	set_timer0();
	init_timer1();
	set_timer1(60);


	sei();

	printMenu();

	while (1)
	{
		
	}

}

ISR(TIMER0_OVF_vect)
{
	if (timer0_remaining_ovf != 0)
	{
		timer0_remaining_ovf = timer0_remaining_ovf - 1;
		
	}
}

ISR(TIMER0_COMPA_vect)
{
	if (timer0_remaining_ovf == 0)
	{
		/* Print display and reset counter */
		PORTB ^= (1<<PORTB5);
		set_timer0();
		
	}
}

ISR(TIMER1_OVF_vect)
{
	usart_puts("\n\rTimer 1 overflow\n\r");
	if (timer1_remaining_ovf != 0)
	{
		timer1_remaining_ovf -= 1;
	}
	
}

ISR(TIMER1_COMPA_vect)
{
	usart_puts("\n\rTimer 1 triggered\n\r");
	if (timer1_remaining_ovf == 0)
	{
		timer1IsDone = 1;
		usart_puts("\n\rTimer 1 Finish\n\r");
		/* Stop timer 1 */
		TCCR1B = 0x00;
	}
}
