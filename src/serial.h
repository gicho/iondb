/*
 * serial.h
 *
 *  Created on: Sep 13, 2014
 *      Author: workstation
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define F_CPU 16000000UL
#define USART_BAUDRATE 19200
#define BAUD_PRESCALE ((( F_CPU / ( USART_BAUDRATE * 16UL ))) - 1)
#define BUFFER_SIZE 32

typedef struct uart_buffer
{
	unsigned char 	buffer[BUFFER_SIZE];

	unsigned char 	head;
	unsigned char 	tail;
	unsigned char 	count;

} uart_buffer_t;

static volatile uart_buffer_t tx_buffer;
static volatile uart_buffer_t rx_buffer;

int uart_putchar(
	char c,
	FILE *stream);

char uart_getchar();

void uart_init();

#ifdef atmega328p
ISR(USART_RX_vect)
{
	if (rx_buffer.tail != (rx_buffer.head + 1)%BUFFER_SIZE)
		{
			rx_buffer.buffer[rx_buffer.head++] = UDR0;
			if (rx_buffer.head >= BUFFER_SIZE)
			rx_buffer.head = 0;
		}
}

ISR(USART_TX_vect)
{
	while (tx_buffer.head != tx_buffer.tail)
		{
			while(!(UCSR0A & (1<<UDRE0)));
			UDR0 = tx_buffer.buffer[tx_buffer.tail];   //write out the data
			tx_buffer.tail = (tx_buffer.tail + 1)%BUFFER_SIZE;
		}
}
#endif

#ifdef atmega2560
ISR(USART0_RX_vect)
{
	if (rx_buffer.tail != (rx_buffer.head + 1)%BUFFER_SIZE)
	{
		rx_buffer.buffer[rx_buffer.head++] = UDR0;
		if (rx_buffer.head >= BUFFER_SIZE)
		rx_buffer.head = 0;
	}
}

ISR(USART0_TX_vect)
{
	while (tx_buffer.head != tx_buffer.tail)
		{
			while(!(UCSR0A & (1<<UDRE0)));
			UDR0 = tx_buffer.buffer[tx_buffer.tail];   //write out the data
			tx_buffer.tail = (tx_buffer.tail + 1)%BUFFER_SIZE;
		}
}
#endif


char
uart_getchar()
{
	char c;
	while (rx_buffer.head == rx_buffer.tail);
	c = rx_buffer.buffer[rx_buffer.tail];   //write out the data
	rx_buffer.tail = (rx_buffer.tail + 1)%BUFFER_SIZE;
	return c;
}

int
uart_putchar(char c, FILE *stream)
{
	while (tx_buffer.tail == (tx_buffer.head + 1)%BUFFER_SIZE)
		{} //spin weight
	if ((tx_buffer.head == tx_buffer.tail)) 	 //if the buffer is empty just write out data
	{
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = c;
	}
	else
	{
		tx_buffer.buffer[tx_buffer.head++] = c;
		if (tx_buffer.head >= BUFFER_SIZE)
			tx_buffer.head = 0;
	}

	return 0;
}

void
uart_init()
	{
		/* reset buffers */
		if (&tx_buffer != NULL)
		{
				tx_buffer.head = 0;
				tx_buffer.tail = 0;
		}
		if (&rx_buffer != NULL)
		{
				rx_buffer.head = 0;
				rx_buffer.tail = 0;
		}

#ifdef atmega328p
		/*Set baud rate */
		UBRR0H = BAUD_PRESCALE>>8;
		UBRR0L = BAUD_PRESCALE;
		/*Enable receiver and transmitter */
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);
		/* Set frame format: 8data, 1stop bit */
		UCSR0C = (0<<USBS0)|(3<<UCSZ00);
		/* enable interrupt */
		UCSR0B |= (1 << RXCIE0 ); // Enable the USART Recieve Complete interrupt ( USART_RXC )
#endif

#ifdef atmega2560
		/* Set baud rate */
		UBRR0H = BAUD_PRESCALE>>8;
		UBRR0L = BAUD_PRESCALE;
		/* Enable receiver and transmitter */
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);
		/* Set frame format: 8data, 1s */
		UCSR0C = (0<<USBS0)|(3<<UCSZ00);
		/* enable interrupt */
		UCSR0B |= (1 << RXCIE0 ); // Enable the USART Recieve Complete interrupt ( USART_RXC )
#endif
}

#endif /* SERIAL_H_ */
