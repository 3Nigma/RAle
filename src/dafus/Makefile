# ======================================================================
# Common Makefile for USBtiny applications
#
# Macros to be defined before including this file:
#
# USBTINY	- the location of this directory
# TARGET_ARCH	- gcc -mmcu= option with AVR device type
# OBJECTS	- the objects in addition to the USBtiny objects
# FLASH_CMD	- command to upload main.hex to flash
# FUSES_CMD	- command to program the fuse bytes
# STACK		- maximum stack size (optional)
# FLASH		- flash size (optional)
# SRAM		- SRAM size (optional)
# SCHEM		- Postscript version of the schematic to be generated
#
# Copyright 2006-2010 Dick Streefland
#
# This is free software, licensed under the terms of the GNU General
# Public License as published by the Free Software Foundation.
# ======================================================================

USBTINY		= ./
TARGET_ARCH	= -mmcu=attiny2313
OBJECTS		= main.o
FLASH_CMD	= avrdude -pt2313 -U flash:w:main.hex
FUSES_CMD	= avrdude -pt2313 -U hfuse:w:0xdf:m -U lfuse:w:0xef:m
STACK		= 32
FLASH		= 2048
SRAM		= 128
check	    = $(shell $(CC) $1 -c -xc /dev/null -o/dev/null 2>/dev/null && echo $1)

CC	= avr-gcc
OPTIM	= -Os -ffunction-sections $(call check,-fno-split-wide-types)
CFLAGS	= -g -Wall -I. -I$(USBTINY) $(OPTIM)
LDFLAGS	= -g -Wl,--relax,--gc-sections
MODULES = crc.o int.o usb.o $(OBJECTS)
UTIL	= $(USBTINY)

main.hex:

all:		main.hex $(SCHEM)

clean:
	rm -f main.elf main.hex *.o

clobber:	clean
	rm -f main.hex $(SCHEM)

main.elf:	$(MODULES)
	$(LINK.o) -o $@ $(MODULES)

main.hex:	main.elf $(UTIL)/check.py
	@python $(UTIL)/check.py main.elf $(STACK) $(FLASH) $(SRAM)
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

check:		main.elf $(UTIL)/check.py
	@python $(UTIL)/check.py main.elf $(STACK) $(FLASH) $(SRAM)

disasm:		main.elf
	avr-objdump -S main.elf

flash:		main.hex
	$(FLASH_CMD)

fuses:
	$(FUSES_CMD)

crc.o:		$(USBTINY)/crc.S $(USBTINY)/def.h usbtiny.h
	$(COMPILE.c) $(USBTINY)/crc.S
int.o:		$(USBTINY)/int.S $(USBTINY)/def.h usbtiny.h
	$(COMPILE.c) $(USBTINY)/int.S
usb.o:		$(USBTINY)/usb.c $(USBTINY)/def.h $(USBTINY)/usb.h usbtiny.h
	$(COMPILE.c) $(USBTINY)/usb.c

main.o:		$(USBTINY)/usb.h

.PHONY: clean check all
