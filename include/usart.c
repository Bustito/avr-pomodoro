#include "usart.h"

void usart_init(uint32_t baud)
{
    uint16_t ubrr = F_CPU/16/baud-1;	/* Calculate UBRR */
	UBRR0H = (uint8_t)(ubrr>>8);		/* Get UBRR high byte */
	UBRR0L = (uint8_t)(ubrr);			/* Get UBRR low byte */
	UCSR0B = (1<<TXEN0);				/* Enable transmit module */
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	/* Select protocol config */
}

void usart_tx(char data)
{
	while (!(UCSR0A & (1<<UDRE0)));		/* Wait for previous transmission to end */
	UDR0 = data;						/* Load data into buffer, and send it */
}

void usart_write(char *data)
{
	char *ptr = data;					/* Get pointer of first byte of data*/
	while (*ptr != '\0')				/* While data isn't NULL */
	{
		usart_tx(*ptr);				/* Send it */
		ptr++;							/* Move to the next byte */
	}
}