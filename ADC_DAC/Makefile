GCC_DIR =  /opt/ti/mspgcc/bin
SUPPORT_FILE_DIRECTORY = /opt/ti/mspgcc/include

DEVICE  = msp430g2253
CC      = $(GCC_DIR)/msp430-elf-gcc
GDB     = $(GCC_DIR)/msp430-elf-gdb
OBJCOPY = $(GCC_DIR)/msp430-elf-objcopy
FLASH   = MSP430Flasher

CFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -mmcu=$(DEVICE) -O2 -g -Werror
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY) -T $(DEVICE).ld

all: program

temp.o: temp.c temp.h uart.h
	$(CC) $(CFLAGS) -c temp.c -o $@

rx_queue.o: rx_queue.c rx_queue.h
	$(CC) $(CFLAGS) -c rx_queue.c -o $@

uart.o: uart.c uart.h rx_queue.h fan.h main.h
	$(CC) $(CFLAGS) -c uart.c -o $@

fan.o: fan.c fan.h uart.h
	$(CC) $(CFLAGS) -c fan.c -o $@

main.o: main.c main.h uart.h temp.h fan.h
	$(CC) $(CFLAGS) -c main.c -o $@

${DEVICE}.elf: main.o uart.o temp.o rx_queue.o fan.o
	$(CC) $(CFLAGS) $(LFLAGS) main.o uart.o temp.o rx_queue.o fan.o -o $(DEVICE).elf

${DEVICE}.hex: ${DEVICE}.elf
	$(OBJCOPY) -O ihex $(DEVICE).elf $(DEVICE).hex

build: ${DEVICE}.hex

program: ${DEVICE}.hex
	#$(FLASH) -n $(DEVICE) -w $(DEVICE).hex -v -z '[VCC]'
	$(FLASH) -w $(DEVICE).hex -v -z '[VCC]'

debug: ${DEVICE}.elf
	#$(GDB) $(DEVICE).elf
	$(GDB) -x debug.gdb

clean:
	rm *.o *.elf *.hex

