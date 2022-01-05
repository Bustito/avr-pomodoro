#ifndef USART_H
#define USART_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define __AVR_ATmega328P__
#include <avr/io.h>

/**
 * @brief Initializes USART communication
 * 
 * @param baud Desired baudrate
 */
void usart_init(uint32_t baud);

/**
 * @brief Send one byte of data
 * 
 * @param data 
 */
void usart_tx(char data);

/**
 * @brief Send multiple bytes of data
 * 
 * @param data String of information
 */
void usart_write(char *data);

#endif