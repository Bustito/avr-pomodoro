#include "ds3231.h"

void ds3231_write(uint8_t reg, uint8_t data)
{
	i2c_start(ADDRESS<<1);
	i2c_write(reg);
	i2c_write(data);
	i2c_stop();
}

void ds3231_read_write(uint8_t reg, uint8_t mask, short action)
{
	uint8_t aux;
	i2c_start(ADDRESS<<1);
	if (!i2c_write(reg))
	{
		i2c_rep_start((ADDRESS<<1)+1);
		aux = i2c_readNak();
		if (action == SET)	//Set
		{
			aux |= mask;
		}else	//Clear
		{
			aux &= ~mask;
		}
		i2c_start(ADDRESS<<1);
		i2c_write(reg);
		i2c_write(aux);
	}
	i2c_stop();
}

uint8_t ds3231_read(uint8_t reg)
{
	uint8_t aux;
	i2c_start(ADDRESS<<1);
	i2c_write(reg);
	i2c_rep_start((ADDRESS<<1)+1);
	aux = i2c_readNak();
	i2c_stop();
	return aux;
}

uint8_t dec2bcd(char num)
{
	return ((num/10 * 16) + (num % 10));
}

uint8_t bcd2dec(char num)
{
	return ((num/16 * 10) + (num % 16));
}

void setTime(rtc_t rtc)
{
	i2c_start(ADDRESS<<1);
	i2c_write(0x00);
	i2c_write(dec2bcd(rtc.sec));
	i2c_write(dec2bcd(rtc.min));
	i2c_write(dec2bcd(rtc.hour));
	i2c_write(rtc.weekDay);
	i2c_write(dec2bcd(rtc.date));
	i2c_write(dec2bcd(rtc.month));
	i2c_write(dec2bcd(rtc.year));
	i2c_stop();
}

rtc_t getTime()
{
	rtc_t aux;
	
	i2c_start(ADDRESS<<1);
	i2c_write(0x00);
	i2c_rep_start((ADDRESS<<1)+1);
	aux.sec = i2c_readAck();
	aux.min = i2c_readAck();
	aux.hour = i2c_readAck();
	aux.weekDay = i2c_readAck();
	aux.date = i2c_readAck();
	aux.month = i2c_readAck();
	aux.year = i2c_readNak();
	i2c_stop();

	aux.sec = bcd2dec(aux.sec);
	aux.min = bcd2dec(aux.min);
	aux.hour = bcd2dec(aux.hour&0x3f);
	aux.date = bcd2dec(aux.date);
	aux.month = bcd2dec(aux.month&0x1f);
	aux.year = bcd2dec(aux.year);
	aux.epoch = convert2Epoch(aux);

	return aux;
}

void ds3231_init()
{
	ds3231_read_write(RTC_STATUS, (1<<A2F)|(1<<A1F), CLEAR);	//Clear ds3231 interrupts flag

	ds3231_write(RTC_CONTROL, (1<<INTCN));	// Configure to enable alarms interrupt

}

void setAlarm(rtc_t time, uint8_t min)
{
	uint8_t aux = time.min+min;
	if (aux>=60)
	{
		aux -= 60;
	}
	aux = dec2bcd(aux);

	ds3231_write(ALARM_2_MINUTES, (ds3231_read(ALARM_2_MINUTES)&0x80)|aux);
}

uint32_t convert2Epoch(rtc_t time)
{
	time_t epoch = UNIX_OFFSET;
	struct tm tmSet;
	tmSet.tm_year = time.year+100;
	tmSet.tm_mon = time.month-1;
	tmSet.tm_mday = time.date;
	tmSet.tm_hour = time.hour;
	tmSet.tm_min = time.min;
	tmSet.tm_sec = time.sec;
	tmSet.tm_isdst = -1;
	epoch += mk_gmtime(&tmSet);
	return epoch;
}

