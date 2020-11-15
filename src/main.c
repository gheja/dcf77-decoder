#include <avr/io.h>
#include <util/delay.h>

#include "defines.h"

#define _BIT(x) ((buffer[x / 8] & (1 << (x % 8))) != 0)
#define _BIT_SET(x) (buffer[x / 8] |= (1 << (x % 8)))

#ifdef SERIAL
#include <stdio.h>
#include "serial.h"
#endif

#ifdef SERIAL
	char s[50];
#endif

uint8_t buffer[8];
uint8_t index;
uint8_t complete;

uint8_t parity(uint8_t a, uint8_t b)
{
	uint8_t i;
	uint8_t n;
	
	n = 0;
	
	for (i=a; i<=b; i++)
	{
		n ^= _BIT(i);
	}
	
	return n;
}

uint8_t bcd(uint8_t a, uint8_t b)
{
	uint8_t i;
	uint8_t n;
	
	n = 0;
	
	for (i=a; i<=b; i++)
	{
		if (_BIT(i))
		{
			n += 1 << (i - a);
		}
	}
	
	return n;
}

void clear()
{
	buffer[0] = 0;
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = 0;
	buffer[4] = 0;
	buffer[5] = 0;
	buffer[6] = 0;
	buffer[7] = 0;
}

void dump()
{
	uint8_t i;
	
	for (i=0; i<60; i++)
	{
		sprintf(s, "%d", _BIT(i));
		USART_TransmitString(s);
	}
	
	USART_TransmitString(" ");
}

void decode()
{
	if (_BIT(0) != 0)
	{
		// USART_TransmitString("invalid 1\r\n");
		USART_TransmitString("e1 ");
		// return;
	}
	
	if (_BIT(20) != 1)
	{
		// USART_TransmitString("invalid 2\r\n");
		USART_TransmitString("e2 ");
		// return;
	}
	
	if (parity(21, 28) != 0)
	{
		// USART_TransmitString("invalid parity 1\r\n");
		USART_TransmitString("p1 ");
		// return;
	}
	
	if (parity(29, 35) != 0)
	{
		// USART_TransmitString("invalid parity 2\r\n");
		USART_TransmitString("p2 ");
		// return;
	}
	
	if (parity(36, 58) != 0)
	{
		// USART_TransmitString("invalid parity 3\r\n");
		USART_TransmitString("p3 ");
		// return;
	}
	
	sprintf(s, "20%02d-%02d-%02d %02d:%02d:%02d %s\r\n", 
		bcd(50, 53) + bcd(54, 57) * 10,
		bcd(45, 48) + bcd(49, 49) * 10,
		bcd(36, 39) + bcd(40, 41) * 10,
		bcd(29, 32) + bcd(33, 34) * 10,
		bcd(21, 24) + bcd(25, 27) * 10,
		0,
		(_BIT(17) && !_BIT(18)) ? "CEST" : (
			(!_BIT(17) && _BIT(18)) ? "CET" : "???"
		)
	);
	USART_TransmitString(s);
}

inline void init()
{
	DDR = 0x00;
	
#ifdef SERIAL
	USART_Init();
	
	USART_TransmitString("Hello!\r\n");
#endif
}


uint8_t get_value()
{
	// with some noise filtered
	
	uint8_t i;
	uint8_t n;
	
	n = 0;
	
	for (i=0; i<200; i++)
	{
		if (PIN_IN & _BV(PIN_DCF))
		{
			n++;
		}
		
		_delay_us(50);
	}
	
	if (n > 150)
	{
		return 1;
	}
	
	return 0;
}

const uint8_t STATE_WAIT_FOR_SYNC = 0;
const uint8_t STATE_SIGNAL = 1;

const uint8_t VALUE_0 = 0;
const uint8_t VALUE_1 = 1;
const uint8_t VALUE_SYNC = 2;
const uint8_t VALUE_INVALID = 3;
const uint8_t VALUE_NONE = 4;

uint8_t state;


int main()
{
	uint8_t value;
	uint8_t value2;
	uint8_t count_low;
	uint8_t count_high;
	
	init();
	
	complete = 0;
	state = STATE_WAIT_FOR_SYNC;
	
	while (1)
	{
		value = get_value();
		value2 = VALUE_NONE;
		
		if (value == 0)
		{
			// prevent overflow
			if (count_low < 255)
			{
				count_low++;
			}
			
			if (count_low > 5)
			{
				count_high = 0;
			}
		}
		else if (value == 1)
		{
			if (count_high < 255)
			{
				count_high++;
			}
			
			if (count_high > 5)
			{
				// ignore some noise
				if (count_low > 20)
				{
					if (count_low < 70)
					{
						value2 = VALUE_INVALID;
					}
					else if (count_low < 85)
					{
						value2 = VALUE_1;
					}
					else if (count_low < 95)
					{
						value2 = VALUE_0;
					}
					else if (count_low < 170)
					{
						value2 = VALUE_INVALID;
					}
					else if (count_low < 190)
					{
						value2 = VALUE_SYNC;
					}
					else
					{
						value2 = VALUE_INVALID;
					}

#ifdef DEBUG
					sprintf(s, "l:%03d ", count_low);
					USART_TransmitString(s);
#endif
				}
				
				count_low = 0;
			}
		}
		
		if (value2 != VALUE_NONE)
		{
#ifdef DEBUG
			sprintf(s, "s:%d i:%02d v:%d\r\n", state, index, value2);
			USART_TransmitString(s);
#endif
			
			if (state == STATE_WAIT_FOR_SYNC)
			{
				if (value2 == VALUE_SYNC)
				{
#ifdef DEBUG
					dump();
#endif
					
					if (!complete)
					{
#ifdef DEBUG
						USART_TransmitString("incomplete ");
#endif
					}
					else
					{
						decode();
					}
					
					clear();
					state = STATE_SIGNAL;
					index = 0;
					complete = 0;
#ifdef DEBUG
					USART_TransmitString("sync ok\r\n");
#endif
				}
			}
			else if (state == STATE_SIGNAL)
			{
				if (value2 == VALUE_1)
				{
					_BIT_SET(index);
				}
				else if (value2 == VALUE_0)
				{
					//
				}
				else
				{
#ifdef DEBUG
					USART_TransmitString("restart\r\n");
#endif
					state = STATE_WAIT_FOR_SYNC;
				}
				
				if (state == STATE_SIGNAL && index == 57)
				{
					complete = 1;
					
					// restart
#ifdef DEBUG
					USART_TransmitString("completed\r\n");
#endif
					state = STATE_WAIT_FOR_SYNC;
				}
				
				index++;
			}
		}
	}
	
	return 0;
}
