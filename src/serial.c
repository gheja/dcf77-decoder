#include <avr/io.h>
#include <avr/interrupt.h>

#include "serial.h"

void USART_Init()
{
	unsigned int ubrr = F_CPU / 16 / 9600 - 1;
	
	/* Set baud rate */
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;
	
	/* Enable receiver and transmitter */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1 << UDRE0)));
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void USART_TransmitString(unsigned char a[])
{
	unsigned char i;
	
	for (i=0; a[i] != '\0'; i++)
	{
		USART_Transmit(a[i]);
	}
}
