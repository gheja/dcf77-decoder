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
	uint8_t t;
	
	init();
	
	t = 0;
	
	while (1)
	{
		value = PIN_IN & _BV(PIN_DCF);
		t++;
		
		// USART_TransmitString(value ? "1" : "0");
		USART_TransmitString(value ? "#" : "_");
		
		if (t == 200)
		{
			USART_TransmitString("\r\n");
			t = 0;
		}
		
		_delay_ms(5);
	}
	
	return 0;
}
