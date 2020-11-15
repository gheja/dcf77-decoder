#include <avr/io.h>
#include <util/delay.h>

#include "defines.h"
#include "macros.h"

#ifdef SERIAL
#include <stdio.h>
#include "serial.h"
#endif

inline void init()
{
	// wdt_disable();
	
	DDR = 0x00;
	
#ifdef SERIAL
	USART_Init();
	
	USART_TransmitString("Hello!\r\n");
#endif
}

int main()
{
	uint8_t value;
	uint8_t last_value;
	uint8_t t;
	uint8_t count_high;
	uint8_t count_total;
	char s[20];
	
	init();
	
	t = 0;
	count_high = 0;
	count_total = 0;
	last_value = 0;
	value = 0;
	
	while (1)
	{
		value = PIN_IN & _BV(PIN_DCF);
		_delay_ms(5);
		t++;
		
		if (value != last_value)
		{
			if (!value)
			{
				if (count_total == 255)
				{
					sprintf(s, "xxx/xxx\r\n");
				}
				else
				{
					sprintf(s, "%03d/%03d\r\n", count_high, count_total);
				}
				USART_TransmitString(s);
				count_total = 0;
			}
			
			count_high = 0;
		}
		
		if (value && count_high < 255)
		{
			count_high++;
		}
		
		if (count_total < 255)
		{
			count_total++;
		}

		last_value = value;
	}
	
	return 0;
}
