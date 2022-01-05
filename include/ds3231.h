#ifndef DS3231_H
#define DS3231_H

#define __AVR_ATmega328P__
#include <avr/io.h>
#include <time.h>

#include "i2cmaster.h"

#define SET		1
#define	CLEAR	~SET

#define	ADDRESS 0x68

#define	RTC_CONTROL	0x0E
#define	EOSC	7
#define BBSQW 	6
#define	CONV	5
#define RS2		4
#define	RS1		3
#define	INTCN	2
#define A2IE	1
#define	A1IE	0

#define RTC_STATUS 	0x0F
#define OSF		7
#define EN32kHz 3
#define BSY 	2
#define A2F 	1
#define A1F		0

#define	ALARM_1_SECONDS	0x07
#define	A1M1	7

#define	ALARM_1_MINUTES	0x08
#define	A1M2	7

#define	ALARM_1_HOURS	0x09
#define	A1M3	7

#define	ALARM_1_DATE	0x0A
#define	A1M4	7

#define	ALARM_2_MINUTES	0x0B
#define	A2M2	7

#define	ALARM_2_HOURS	0x0C
#define	A2M3	7

#define	ALARM_2_DATE	0x0D
#define	A2M4	7

typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t weekDay;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint32_t epoch;

}rtc_t;

uint8_t ds3231_read(uint8_t reg);

void ds3231_write(uint8_t reg, uint8_t data);

void ds3231_read_write(uint8_t reg, uint8_t mask, short action);

uint32_t convert2Epoch(rtc_t time);

void ds3231_init();

void setTime(rtc_t time);

rtc_t getTime();

void setAlarm(rtc_t time, uint8_t min);

uint8_t dec2bcd(char num);

uint8_t bcd2dec(char num);
#endif