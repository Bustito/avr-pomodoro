/**
 * @file sh1106.h
 * @author Martin Bustos (bustos.martin@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef SH1106_H
#define SH2206_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "i2cmaster.h"

#define SH1106_ADDR 0x78

#define W_COMMAND 0x00
#define W_DATA 0x40

#define SET_PAGE 0xB0
#define SET_COL_L 0x00
#define SET_COL_H 0x10

/**
 * @brief Initializes the screen, with all pixels off
 * 
 * @retval 0 init successfull
 * @retval 1 if init failed
 */
uint8_t sh1106_init ();

/**
 * @brief Set the active page of the display
 * 
 * @param page (0 to 7)
 * @retval 0 command successfull
 * @retval 1 command failed
 */
uint8_t setPage (uint8_t page);

/**
 * @brief Set the active column of the display
 * 
 * @param column (0 to 132)
 * @retval 0 command successfull
 * @retval 1 command failed 
 */
uint8_t setColumn (uint8_t column);

/**
 * @brief Clear the entire screen
 * 
 * @retval 0 command successfull
 * @retval 1 command failed
 */
uint8_t clearPage ();

/**
 * @brief Print a full bitmap on the screen
 * 
 * @param map 
 * @retval 0 map printed successfully
 * @retval 1 map print failed 
 */
uint8_t printBmp (const uint8_t map[8][128]);

#endif