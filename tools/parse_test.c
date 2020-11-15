#include <stdio.h>
#include <stdint.h>

uint64_t buffer0;

#define _BIT(x,y) ((x & (1l << (60 - y))) != 0)
#define _error(x) { printf(x); return; }

uint8_t parity(uint64_t x, uint8_t a, uint8_t b)
{
	uint8_t i;
	uint8_t n;
	
	n = 0;
	
	for (i=a; i<=b; i++)
	{
		printf("%d ", _BIT(x, i));
		
		n ^= _BIT(x, i);
	}
	
	printf(" -- %d\n", n);
	
	return n;
}

uint8_t bcd(uint64_t x, uint8_t a, uint8_t b)
{
	uint8_t i;
	uint8_t n;
	
	n = 0;
	
	for (i=a; i<=b; i++)
	{
		printf("%d %d %d %d\n", _BIT(x, i), i, a, 1 << (i - a));
		
		if (_BIT(x, i))
		{
			n += 1 << (i - a);
		}
	}
	
	return n;
}

void receive(uint8_t x)
{
	if (x != 0 && x != 1)
	{
		return;
	}
	
	buffer0 = buffer0 << 1;
	buffer0 |= x & 0x01;
	
	printf("%016x\n", buffer0);
	
	if (_BIT(buffer0, 0) != 0)
	{
		_error("invalid 1\n");
	}
	
	if (_BIT(buffer0, 20) != 1)
	{
		_error("invalid 2\n");
	}
	
	if (parity(buffer0, 21, 28))
	{
		_error("invalid parity 1\n");
	}
	
	if (parity(buffer0, 29, 35))
	{
		_error("invalid parity 2\n");
	}
	
	if (parity(buffer0, 36, 58))
	{
		_error("invalid parity 3\n");
	}
	
/*
	printf("m: %d\n", bcd(buffer0, 21, 24) + bcd(buffer0, 25, 27) * 10);
	printf("h: %d\n", bcd(buffer0, 29, 32) + bcd(buffer0, 33, 34) * 10);
	printf("d: %d\n", bcd(buffer0, 36, 39) + bcd(buffer0, 40, 41) * 10);
	printf("m: %d\n", bcd(buffer0, 45, 48) + bcd(buffer0, 49, 49) * 10);
	printf("y: %d\n", bcd(buffer0, 50, 53) + bcd(buffer0, 54, 57) * 10);
	printf("\n");
*/
	printf("20%02d-%02d-%02d %02d:%02d:%02d\r\n", 
		bcd(buffer0, 50, 53) + bcd(buffer0, 54, 57) * 10,
		bcd(buffer0, 45, 48) + bcd(buffer0, 49, 49) * 10,
		bcd(buffer0, 36, 39) + bcd(buffer0, 40, 41) * 10,
		bcd(buffer0, 29, 32) + bcd(buffer0, 33, 34) * 10,
		bcd(buffer0, 21, 24) + bcd(buffer0, 25, 27) * 10,
		1
	);
	
	printf("valid!\n");
}

void main()
{
	uint8_t i;
	// uint8_t data[] = { 2,2,2,0,0,1,1,0,1,1,1,0,0,0,0,1,0,1,1,1,0,0,0,1,0,1,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,1,1,1,0,1,1,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,1,1,0,1,0,0,1,1,1,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,1,1,1,0,0,1,1,0,0,1,1,0,0,0,1,0,1,0,1,1,0,0,1,0,1,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,0,1,255 };
	// uint8_t data[] = { 0,0,1,0,1,1,1,1,1,1,0,0,0,1,0,0,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,1,1,1,0,0,0,0,1,0,1,0,0,0,0,255 };
	// uint8_t data[] = { 0,0,0,1,0,0,0,0,1,1,1,1,1,1, 0,1,0,1,0,1,0,1,1,1,1,1,1,0, 0,0,1,1,0,0,0,1,0,1,1,1,1,0, 1,1,1,0,1,0,0,0, 0,1,0,0,0,0,0,0, 1,0,0,0,0,1,0,0, 1,1,1,0,0, 1,0,0, 0,0,1,0,1,0,0,0,255 };
	uint8_t data[] = { 1,0,0,0,1,0,0,0,1,1, 0,0,1,0,0,0,1,0,0,1,1,0,0, 1,1,0,1,0,1,0,0,0,0, 1,0,1,0,1,0,1,0,1,1,1, 0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,255 };
	
	printf("%d\n", ((0xefffffffffffffff & (1l << 64)) != 0l));
	
	i = 0;
	
	while (data[i] != 255)
	{
		receive(data[i]);
		i++;
	}
	
	printf("%d\n", data[0]);
}