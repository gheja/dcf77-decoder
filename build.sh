#!/bin/bash -e

program="test"
programmer="usbasp"
# programmer="usbtiny"
profile="atmega328p"
# profile="trinket"
avrdude_extra=""
avrdude_fuses=""

if [ "$profile" == "attiny13" ]; then
	avr_mcu="attiny13"
	avrdude_part="t13"
	f_cpu="8000000"
elif [ "$profile" == "attiny84" ]; then
	avr_mcu="attiny84"
	avrdude_part="t84"
	f_cpu="1000000"
elif [ "$profile" == "attiny85" ]; then
	avr_mcu="attiny85"
	avrdude_part="t85"
	f_cpu="1000000"
elif [ "$profile" == "atmega328p" ]; then
	avr_mcu="atmega328p"
	avrdude_part="m328p"
	f_cpu="1000000"
	avrdude_extra="-B 10"
	
	# ATMega328P defaults (E:FF, H:D9, L:62)
	avrdude_fuses="-U lfuse:w:0x62:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m"
elif [ "$profile" == "trinket" ]; then
	programmer="usbtiny"
	avr_mcu="attiny85"
	avrdude_part="t85t"
	f_cpu="8000000"
else
	echo "Unknown profile, exiting."
	exit 1
fi

cd src

avr-gcc -std=c99 -g -Os -mmcu=${avr_mcu} -o ${program}.o -DF_CPU=${f_cpu} \
	serial.c main.c

avr-objcopy -j .text -j .data -O ihex ${program}.o ${program}.hex
avr-size -d --mcu=${avr_mcu} ${program}.o
avr-objdump -d ${program}.o > ${program}.asm

# sudo avrdude -c ${programmer} -p ${avrdude_part} ${avrdude_extra} ${avrdude_fuses}

sudo avrdude -c ${programmer} -p ${avrdude_part} ${avrdude_extra} -U flash:w:${program}.hex
