#define __AVR_ATmega328P__
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>

#include "i2cmaster.h"
#include "sh1106.h"
#include "ds3231.h"
#include "usart.h"

#include "bitMaps.h"

typedef struct profile
{
	uint8_t workTime;
	uint8_t shortBreakTime;
	uint8_t longBreakTime;
	uint8_t interval;

}profile;
typedef struct timer
{
	uint8_t minutes;
	uint8_t seconds;
	uint8_t interval;
}timer;

volatile profile actualProfile;
volatile timer myTimer;

volatile uint8_t state = 0;					/* nothing = 0, workTime = 1, shortBreakTime = 2, longBreakTime = 3 */

volatile uint8_t interruptMode = 0;			/* Button B = 0, Seconds interrupt = 1, Minutes interrupt = 2 */
volatile uint16_t menuIndex = 0x00;			/* Level 0 Index 0 */

char buff[40];

void setTimer(uint8_t minutes);
void stopTimer();

ISR(INT0_vect);
ISR(INT1_vect);

int main (void) {

	PORTD |= (1<<PORTD2)|(1<<PORTD1);

	i2c_init();
	sh1106_init();
	usart_init(9600);

	printBmp(mainMenu);

	/* Config sleep mode */
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	/* Config external interrupts */
	EICRA = (1<<ISC01)|(1<<ISC11);
	EIMSK = (1<<INT0)|(1<<INT1);

	/* Load default values */
	/* Add EEPROM funtionality */
	actualProfile.longBreakTime = 15;
	actualProfile.shortBreakTime = 5;
	actualProfile.workTime = 30;
	actualProfile.interval = 2;
	

	sei();
	while (1)
	{

	}
	return 0;
}

void setTimer(uint8_t minutes)
{
	/* ALARM 1 INTERRUPTS EVERY SECOND */
	ds3231_write(ALARM_1_SECONDS, 0x80);
	ds3231_write(ALARM_1_MINUTES, 0x80);
	ds3231_write(ALARM_1_HOURS, 0x80);
	ds3231_write(ALARM_1_DATE, 0x80);

	rtc_t aux = getTime();
	aux.min += minutes;
	if (aux.min>59)
	{
		aux.min -= 60;
		aux.hour++;
		if (aux.hour>23)
			aux.hour = 0;
	}

	/* ALARM 2 INTERRUPTS WHEN HOURS AND MINUTES MATCH */
	ds3231_write(ALARM_2_MINUTES, 0x7F&(dec2bcd(aux.min)));		/* A2M2 = 0 */
	ds3231_write(ALARM_2_HOURS, 0x1F&(dec2bcd(aux.hour)));		/* A2M3 = 0 */
	ds3231_write(ALARM_2_DATE, 0x80);							/* A2M4 = 1  DY/DT = 0 */

	ds3231_write(RTC_CONTROL, (1<<INTCN)|(1<<A1IE)|(1<<A2IE));
}

void stopTimer()
{
	ds3231_write(RTC_CONTROL, (1<<INTCN));
}

/* Interrupt for B button and DS3231 alarms */
/* If counter active, DS3231 alarm is active, if not, B button is active */
ISR(INT1_vect)
{
	usart_write("INT1\r\n");
}

ISR(INT0_vect)
{
	usart_write("INT0\r\n");
}