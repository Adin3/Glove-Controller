MCU = atmega328p
F_CPU = 16000000UL
CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS = -Wl,-u,vfprintf -lprintf_flt -lm

main.hex: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o main.elf main.c
	$(OBJCOPY) -O ihex -R .eeprom main.elf main.hex

flash: main.hex
	avrdude -c arduino -p m328p -P /dev/ttyUSB0 -b 115200 -U flash:w:main.hex
