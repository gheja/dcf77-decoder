#!/bin/bash

while [ 1 ]; do
	if lsusb | grep -q 16c0:05dc; then
		./build.sh
		
		while lsusb | grep -q 16c0:05dc; do
			sleep 1
		done
	fi
	
	if [ -e /dev/ttyUSB0 ]; then
		sudo picocom --baud 9600 /dev/ttyUSB0
	fi
	
	sleep 1
done
