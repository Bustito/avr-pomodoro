/**********************************************
 * MAIN PROGRAM
 * 
 * Autor: Martin Bustos
 * Date: 12/8/2021
 * 
 * Platform: ATmega328P
 * 
 **********************************************/

#define F_CPU 16000000

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "include/uart.h"

#define BAUD	9600

short mainMenuIsActive = 1;
short configMenuIsActive = 0;
short timerIsActive = 0;

short workConfigIsActive = 0;
short shortBreakConfigIsActive = 0;
short longBreakConfigIsActive = 0;
short intervalConfigIsActive = 0;

volatile short busy = 0;

unsigned char selection = 0;

unsigned char actual_interval = 0;

typedef struct timer{
	unsigned char work_time;
	unsigned char long_break_time;
	unsigned char short_break_time;
	unsigned char interval;
	short autoload;
}timer;

void usart_putc(unsigned char data) {
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

int usart_putcf(char var, FILE *stream) {
    if (var == '\n') usart_putc('\r');
    usart_putc(var);
    return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(usart_putcf, NULL, _FDEV_SETUP_WRITE);

void printMenu(unsigned char selection) {
	printf("\n\r\tMENU\n\r");
	if (selection == 0) {
		printf("->1 - Start timer\n\r2 - Configure timer\n\r3 - Put to sleep\n\n\r");
	} else if (selection == 1) {
		printf("1 - Start timer\n\r->2 - Configure timer\n\r3 - Put to sleep\n\n\r");
	} else if (selection == 2) {
		printf("1 - Start timer\n\r2 - Configure timer\n\r->3 - Put to sleep\n\n\r");
	}
	/*
	printf("Main menu: %d\tConfig menu: %d\n", mainMenuIsActive, configMenuIsActive);
	printf("Selection: %d\nworkConfigIsActive: %d\nshortBreakConfigIsActive: %d\nlongBreakConfigIsActive: %d\nintervalConfigIsActive: %d", selection, workConfigIsActive, shortBreakConfigIsActive, longBreakConfigIsActive, intervalConfigIsActive);
	*/
}

void printConfig(unsigned char selection, unsigned char work_time, unsigned char short_break, unsigned char long_break, unsigned char interval){
	printf("\n\r\tCONFIG\n\r");
	if (selection == 0) {
		printf("->1 - Work time: %d min\n\r2 - Short break: %d min\n\r3 - Long break: %d min\n\r4 - Break interval: %d\n\r5 - Save on rom\n\r6 - Exit\n\n\r", work_time, short_break, long_break, interval);
	} else if (selection == 1) {
		printf("1 - Work time: %d min\n\r->2 - Short break: %d min\n\r3 - Long break: %d min\n\r4 - Break interval: %d\n\r5 - Save on rom\n\r6 - Exit\n\n\r", work_time, short_break, long_break, interval);
	} else if (selection == 2) {
		printf("1 - Work time: %d min\n\r2 - Short break: %d min\n\r->3 - Long break: %d min\n\r4 - Break interval: %d\n\r5 - Save on rom\n\r6 - Exit\n\n\r", work_time, short_break, long_break, interval);
	} else if (selection == 3) {
		printf("1 - Work time: %d min\n\r2 - Short break: %d min\n\r3 - Long break: %d min\n\r->4 - Break interval: %d\n\r5 - Save on rom\n\r6 - Exit\n\n\r", work_time, short_break, long_break, interval);
	} else if (selection == 4) {
		printf("1 - Work time: %d min\n\r2 - Short break: %d min\n\r3 - Long break: %d min\n\r4 - Break interval: %d\n\r->5 - Save on rom\n\r6 - Exit\n\n\r", work_time, short_break, long_break, interval);
	} else if (selection == 5) {
		printf("1 - Work time: %d min\n\r2 - Short break: %d min\n\r3 - Long break: %d min\n\r4 - Break interval: %d\n\r5 - Save on rom\n\r->6 - Exit\n\n\r", work_time, short_break, long_break, interval);
	}
	/*
	printf("Main menu: %d\tConfig menu: %d\n", mainMenuIsActive, configMenuIsActive);
	printf("Selection: %d\nworkConfigIsActive: %d\nshortBreakConfigIsActive: %d\nlongBreakConfigIsActive: %d\nintervalConfigIsActive: %d", selection, workConfigIsActive, shortBreakConfigIsActive, longBreakConfigIsActive, intervalConfigIsActive);
	*/
}

void init_timer0() {
	/* Set timer 0 to normal work */
	TCCR0A = 0x00;

	/* Stop timer 0 */
	TCCR0B = 0x00;
}

void set_timer0 () {

    timer0_ovf = 61;
    timer0_rem = 70;

	/* Set timer 0 counter to 0 */
	TCNT0 = 0;

    /* Set compare unit A to interrupt at the remaining ticks */
	OCR0A = timer0_rem;

	/* Set interrupts for overflow and ouput compare unit A */
	TIMSK0 = (1<<TOIE0);

	/* Set timer 1 clock freq to F_CPU/1024 */
	TCCR0B = (1<<CS02)|(1<<CS00);
}

void init_timer1() {
	/* Set timer 1 to normal work */
	TCCR1A = 0x00;

	/* Stop timer 1 */
	TCCR1B = 0x00;
}

void set_timer1 (uint64_t time) {
    total_ticks = time * 15625;
    overflows = total_ticks / 65535;
    remaining_ticks = total_ticks - overflows * 65535;

    printf("Overflows: %d\tRemaining ticks: %u\n", overflows, remaining_ticks);

	/* Set timer 1 counter to 0 */
	TCNT1H = 0;
	TCNT1L = 0;

    /* Set compare unit A to interrupt at the remaining ticks */
	OCR1A = remaining_ticks;

	/* Set interrupts for overflow and ouput compare unit A */
	TIMSK1 = (1<<TOIE1);

	/* Set timer 1 clock freq to F_CPU/1024 */
	TCCR1B = (1<<CS12)|(1<<CS10);
}

void set_interrupts() {
	/* Configure input and set pullups for them */
	DDRB &= ~((1<<PORTD3)|(1<<PORTD2));
	//PORTB |= (1<<PORTD3)|(1<<PORTD2);

	/* Configure them as interrupts and select mode of operation */
	EIMSK |= (01<<INT0)|(1<<INT1);
	EICRA = (1<<ISC11)|(1<<ISC01);
}

int main() {

	timer mytimer;

	/* Load defaults */
	mytimer.interval = 0;
	mytimer.long_break_time = 0;
	mytimer.short_break_time = 0;
	mytimer.work_time = 0;
	mytimer.autoload = 0;

	/* Stop global interrupts */
	cli();

	DDRB = (1<<DDB5);

	/* Set stream */
	stdout = &mystdout;

	/* Initialize uart */
	uart_init(UART_BAUD_SELECT(BAUD,F_CPU));

	/* Initialize external interrupts */
	//set_interrupts();

	/* Init timers */
	init_timer1();
	init_timer0();

	/* Enable global interrupts */
	sei();

	/* Print main menu */
	printf("\n\rAVR-POMODORO v0.1\n");
	printMenu(selection);

	while (1) {
		while (timerIsActive){
			printf("WORK TIME\n");
            busy = 1;
            set_timer1(mytimer.work_time*60);
            while (busy);
            busy = 1;
            if (actual_interval == mytimer.interval){
                printf("LONG BREAK!\n");
                set_timer1(mytimer.long_break_time*60);
                actual_interval = 0;
            }else{
                printf("SHORT BREAK!\n");
                set_timer1(mytimer.short_break_time*60);
                actual_interval++;
            }
            while (busy);
		}
		
	}
}

ISR (TIMER1_OVF_vect){
    printf("\t\tOVF\n");
    if (overflows!=0)
    {
        overflows -= 1;
    }else{

	    TIMSK1 |= (1<<OCIE1A);
    }
}

ISR (TIMER1_COMPA_vect){
    printf("\t\tFinish\n");
    /* Stop timer */
    TCCR1B = 0x00;
	/* Set interrupts for overflow and ouput compare unit A */
	TIMSK1 &= ~(1<<OCIE1A);
}

ISR (TIMER0_OVF_vect){
    if (timer0_ovf!=0)
    {
        timer0_ovf -= 1;
    }else{

	    TIMSK0 |= (1<<OCIE0A);
    }
}

ISR (TIMER0_COMPA_vect){
    /* Stop timer */
    TCCR0B = 0x00;

    /* Refresh screen */

	/* Set interrupts for overflow and ouput compare unit A */
	TIMSK0 &= ~(1<<OCIE0A);

    set_timer0();
}

/* OK button interrupt */
ISR(INT0_vect) {
	printf("\t\t\tINT0\n");

	if (mainMenuIsActive){

		if (selection == 0) {
			/* Go to start timer */
			mainMenuIsActive = 0;
			timerIsActive = 1;
			/* Start timer 0 */
			set_timer0();

		} else if (selection == 1) {
			/* Go to config menu */
			mainMenuIsActive = 0;
			configMenuIsActive = 1;
			/* Print config menu */
			selection = 0;
			printConfig(selection, mytimer.work_time, mytimer.long_break_time, mytimer.short_break_time, mytimer.interval);
		} else if (selection == 2) {
			/* Go to sleep mode */
			mainMenuIsActive = 0;
			/* Clear main menu */
		}
		selection = 0;
	} else if (configMenuIsActive) {

		if (workConfigIsActive){
			/* Disable configuration */
			workConfigIsActive = 0;
		} else if (shortBreakConfigIsActive) {
			/* Disable configuration */
			shortBreakConfigIsActive = 0;
		} else if (longBreakConfigIsActive) {
			/* Disable configuration */
			longBreakConfigIsActive = 0;
		} else if (intervalConfigIsActive) {
			/* Disable configuration */
			intervalConfigIsActive = 0;

		} else {

			if (selection == 0) {
				/* Modify work time time */
				workConfigIsActive = 1;
			} else if (selection == 1) {
				/* Modify short break time time */
				longBreakConfigIsActive = 1;
			} else if (selection == 2) {
				/* Modify long break time time */
				shortBreakConfigIsActive = 1;
			} else if (selection == 3) {
				/* Modify break time */
				intervalConfigIsActive = 1;
			} else if (selection == 4) {
				/* Save configuration on rom */
				
			} else if (selection == 5) {
				/* Go to main menu */
				configMenuIsActive = 0;
				mainMenuIsActive = 1;
				selection = 0;
				printMenu(selection);
			}
		}	
	} else if (timerIsActive) {

		/* Stop timer 0 */
    	TCCR0B = 0x00;
		
		/* Go to main menu */
		timerIsActive = 0;
		mainMenuIsActive = 1;
		selection = 0;
		printMenu(selection);
	}
}

/* SELECT button interrupt */
ISR(INT1_vect) {
	printf("\t\t\tINT1\n");
	if (mainMenuIsActive) {
		selection++;
		if (selection > 2) {
			selection = 0;
		}
		printMenu(selection);
	} else if (configMenuIsActive) {
		if (workConfigIsActive){
			/* Increment work time */
			mytimer.work_time++;
			if (mytimer.work_time>51) {
				mytimer.work_time = 0;
			}
		} else if (shortBreakConfigIsActive) {
			/* Increment short break time */
			mytimer.short_break_time++;
			if (mytimer.short_break_time>16) {
				mytimer.short_break_time = 0;
			}
		} else if (longBreakConfigIsActive) {
			/* Increment long break time */
			mytimer.long_break_time++;
			if (mytimer.long_break_time>21) {
				mytimer.long_break_time = 0;
			}
		} else if (intervalConfigIsActive) {
			/* Increment interval */
			mytimer.interval++;
			if (mytimer.interval>5) {
				mytimer.interval = 0;
			}

		} else {
			selection++;
			if (selection > 5) {
				selection = 0;
			}
		}
		printConfig(selection, mytimer.work_time, mytimer.long_break_time, mytimer.short_break_time, mytimer.interval);
	}
}