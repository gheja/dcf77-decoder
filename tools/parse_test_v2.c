#include <stdio.h>
#include <stdint.h>

uint8_t buffer[8];

#define _BIT(x) ((buffer[x / 8] & (1 << (x % 8))) != 0)
#define _error(x) { printf(x); return; }

uint8_t parity(uint8_t a, uint8_t b)
{
	uint8_t i;
	uint8_t n;
	
	n = 0;
	
	for (i=a; i<=b; i++)
	{
		printf("%d ", _BIT(i));
		
		n ^= _BIT(i);
	}
	
	printf(" -- %d\n", n);
	
	return n;
}

uint8_t bcd(uint8_t a, uint8_t b)
{
	uint8_t i;
	uint8_t n;
	
	n = 0;
	
	for (i=a; i<=b; i++)
	{
		printf("%d %d %d %d\n", _BIT(i), i, a, 1 << (i - a));
		
		if (_BIT(i))
		{
			n += 1 << (i - a);
		}
	}
	
	return n;
}
void printbits(n)
{
	uint8_t i;
	
	for (i=7; i!=255; i--)
	{
		if (n & (1 << i))
		{
			printf("1");
		}
		else
		{
			printf("0");
		}
	}
}

void receive(uint8_t x)
{
	uint8_t i;
	
	if (x != 0 && x != 1)
	{
		return;
	}
	
/*
	for (i=7; i>0; i--)
	{
		buffer[i] = ((buffer[i] << 1) & 0xff) | ((buffer[i-1] & 0x80) >> 7);
	}
	
	buffer[0] = ((buffer[0] << 1) & 0xff) | (x & 0x01);
*/

	buffer[0] = (buffer[0] >> 1) | ((buffer[1] & 0x01) << 7);
	buffer[1] = (buffer[1] >> 1) | ((buffer[2] & 0x01) << 7);
	buffer[2] = (buffer[2] >> 1) | ((buffer[3] & 0x01) << 7);
	buffer[3] = (buffer[3] >> 1) | ((buffer[4] & 0x01) << 7);
	buffer[4] = (buffer[4] >> 1) | ((buffer[5] & 0x01) << 7);
	buffer[5] = (buffer[5] >> 1) | ((buffer[6] & 0x01) << 7);
	buffer[6] = (buffer[6] >> 1) | ((buffer[7] & 0x01) << 7);
	buffer[7] = (buffer[7] >> 1) | (x << 7);
	
	printbits(buffer[4]);
	printf("-");
	printbits(buffer[3]);
	printf("-");
	printbits(buffer[2]);
	printf("-");
	printbits(buffer[1]);
	printf("-");
	printbits(buffer[0]);
	printf("\n");
	
	printf("%02x%02x%02x%02x%02x%02x%02x%02x\n", buffer[7], buffer[6], buffer[5], buffer[4], buffer[3], buffer[2], buffer[1], buffer[0]);
	
	if (_BIT(0) != 0)
	{
		_error("invalid 1\n");
	}
	
	if (_BIT(20) != 1)
	{
		_error("invalid 2\n");
	}
	
	if (parity(21, 28) != 0)
	{
		_error("invalid parity 1\n");
	}
	
	if (parity(29, 35) != 0)
	{
		_error("invalid parity 2\n");
	}
	
	if (parity(36, 58) != 0)
	{
		_error("invalid parity 3\n");
	}
	
	printf("20%02d-%02d-%02d %02d:%02d:%02d\r\n", 
		bcd(50, 53) + bcd(54, 57) * 10,
		bcd(45, 48) + bcd(49, 49) * 10,
		bcd(36, 39) + bcd(40, 41) * 10,
		bcd(29, 32) + bcd(33, 34) * 10,
		bcd(21, 24) + bcd(25, 27) * 10,
		1
	);
	printf("valid!\n");
}

void main()
{
	uint8_t i;
	
/*
	buffer[7] = 0b11111111;
	buffer[6] = 0b01111111;
	buffer[5] = 0b00111111;
	buffer[4] = 0b00011111;
	buffer[3] = 0b00001111;
	buffer[2] = 0b00000111;
	buffer[1] = 0b00000011;
	buffer[0] = 0b00000001;
	
	for (i=0; i<60; i++)
	{
		printf("%d", _BIT(i));
	}
	
	printf("\n");
	return;
*/	
	
	uint8_t data[] = { 2,2,2,0,0,1,1,0,1,1,1,0,0,0,0,1,0,1,1,1,0,0,0,1,0,1,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,1,1,1,0,1,1,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,1,1,0,1,0,0,1,1,1,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,1,1,1,0,0,1,1,0,0,1,1,0,0,0,1,0,1,0,1,1,0,0,1,0,1,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,0,1,255 };
	// uint8_t data[] = { 0,0,1,0,1,1,1,1,1,1,0,0,0,1,0,0,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,1,1,1,0,0,0,0,1,0,1,0,0,0,0,255 };
	// uint8_t data[] = { 0,0,0,1,0,0,0,0,1,1,1,1,1,1, 0,1,0,1,0,1,0,1,1,1,1,1,1,0, 0,0,1,1,0,0,0,1,0,1,1,1,1,0, 1,1,1,0,1,0,0,0, 0,1,0,0,0,0,0,0, 1,0,0,0,0,1,0,0, 1,1,1,0,0, 1,0,0, 0,0,1,0,1,0,0,0,255 };
	// uint8_t data[] = { 1,0,0,0,1,0,0,0,1,1, 0,0,1,0,0,0,1,0,0,1,1,0,0, 1,1,0,1,0,1,0,0,0,0, 1,0,1,0,1,0,1,0,1,1,1, 0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,255 };
	
	i = 0;
	
	while (data[i] != 255)
	{
		receive(data[i]);
		i++;
	}
	
	printf("%d\n", data[0]);
}