#define F_CPU 16000000

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "include/uart.h"

#define BAUD	9600

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

static FILE mystdout = FDEV_SETUP_STREAM(usart_putcf, NULL, _FDEV_SETUP_WRITE);

void printMenu()
{
	uart_puts("\n\rAVR-POMODORO v0.1\n\n\r\tMENU");
	uart_puts("\n\r1 - Sta");
	uart_puts("rt timer\n\r2 - Configure timer\n\n\r");
}

void init_timer1()
{
	/* Set timer 1 to normal work */
	TCCR1A = 0x00;

	/* Stop timer 1 */
	TCCR1B = 0x00;

	/* Set compare unit A to interrupt at the remaining ticks */
	OCR1A = 62500;

	/* Set interrupts for overflow and ouput compare unit A */
	TIMSK1 = (1<<OCIE1A);
}

void set_timer1 ()
{
	/* Set timer 1 counter to 0 */
	TCNT1H = 0;
	TCNT1L = 0;

	usart_puts("\n\rTimer 1 started\n\r");

	/* Set timer 1 clock freq to F_CPU/256 */
	TCCR1B = (1<<CS12);
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

	init_timer1();

	set_timer1();

	sei();
	while (1)
	{
		
	}

}

ISR(TIMER1_COMPA_vect)
{

	usart_puts("\n\rTimer 1 Finish\n\r");

	/* Stop timer 1 */
	TCCR1B = 0x00;

	set_timer1();
}
