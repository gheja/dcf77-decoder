# DCF77 receiver and decoder

## Scripts

build.sh: build the C code and program the AVR.

build_loop.sh: build and program the AVR if programmer is connected, start terminal if serial converter connected, repeat.

## Samples

Check samples.txt for captured and decoded DCF77 packets.

## Debug mode

Enable debug mode before building in `defines.h` for more verbose messages.

Status line:
```
Example: l:123 s:1 i:05 v:1

l: length of low signal (in cycles)

s: state
  0: waiting for sync
  1: receiving signal

i: index (nth bit is being received)

v: value decoded
  0: bit value 0
  1: bit value 1
  2: sync period
  3: invalid (too long or too short)
  4: none (i.e. timer overflow)
```

Messages:
```
sync ok: sync period found, starting to receive the actual data

restart: unexpected or invalid data received, back to start

completed: successfully received 59 bits of data

incomplete: the data in buffer is incomplete (will not attempt to decode)

e1: start bit is not 0

e2: minute start bit is not 1

p1: first parity bit is invalid (minutes)

p2: second partiy bit is invalid (hours)

p3: third parity bit is invalid (days, day of week, months, years)

e3: minute mark bit is not 0

NOTE: if debug is enabled then the first bit might be invalid as
transmitting the message takes longer time than the first bit and the
edge might not be detected properly
```
