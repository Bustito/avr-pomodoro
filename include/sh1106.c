#include "sh1106.h"

/**
 * @brief Init secuence
 * 
 */
uint8_t init[26] = {
	0xae, 0xd5, 0x80, 0xa8, 0x3f, 0xd3, 0x00, 0x40, 0xad, 0x8b, 0x33, 0x20, 0x00,
	0xa0, 0xc0, 0xda, 0x12, 0x81, 0x80, 0xd9, 0x22, 0xdb, 0x28, 0x2e, 0xa4, 0xa6};

uint8_t writeCmd (uint8_t data)
{
	uint8_t ret = i2c_start(SH1106_ADDR);
	ret |= i2c_write(W_COMMAND);
	if (!ret)
		ret |= i2c_write(data);
	i2c_stop();
	if (ret)
		ret = 1;
	return ret;
}

uint8_t setPage (uint8_t page)
{
	return writeCmd(SET_PAGE | page);
}

uint8_t setColumn (uint8_t col)
{
	uint8_t ret = writeCmd(SET_COL_L | (0x0F & col));
	ret |= writeCmd(SET_COL_H | (col >> 4));
	if (ret)
		ret = 1;
	return ret;
}

uint8_t clearPage ()
{
	uint8_t ret = 0;
	for (uint8_t i = 0; i < 8; i++)
	{
		ret |= setPage(i);
		ret |= setColumn(0);

		i2c_start(SH1106_ADDR + I2C_WRITE);
		ret |= i2c_write(W_DATA);
		for (uint8_t j = 0; j < 132; j++)
		{
			ret |= i2c_write(0x00);
		}
		i2c_stop();
	}
	if (ret)
		ret = 1;
	return ret;
}

uint8_t sh1106_init ()
{
	_delay_ms(100);
	uint8_t ret = 0;
	for (uint8_t i = 0; i < 26; i++)
	{
		ret |= writeCmd(init[i]);
	}
	_delay_ms(100);
	ret |= clearPage();
	ret |= writeCmd(0xaf);
	if (ret)
		ret = 1;
	return ret;
}

uint8_t printBmp (const uint8_t map[8][128]) {
	uint8_t ret = 0;
	for (uint8_t page = 0; page < 8; page++)
	{
		ret |= setPage(page);
		ret |= setColumn(2);
		ret |= i2c_start(SH1106_ADDR+I2C_WRITE);
		ret |= i2c_write(W_DATA);
		for (uint8_t col = 0; col < 128; col++) {
			ret |= i2c_write(pgm_read_byte(&map[page][col]));
		}
		i2c_stop();
	}
	if (ret)
		ret = 1;
	return ret;
}
