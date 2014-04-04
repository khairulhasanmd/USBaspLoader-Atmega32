# Configure the following variables according to your AVR.
# Program the device with
#     make fuse    # to set the clock generator, boot section size etc.
#     make flash   # to load the boot loader into flash
#     make lock    # to protect the boot loader from overwriting

F_CPU = 12000000
DEVICE = atmega32
BOOTLOADER_ADDRESS = 0x7000
FUSEOPT = -U lfuse:w:0xCF:m -U hfuse:w:0xD8:m
LOCKOPT = -U lock:w:0x2f:m

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(DEVICE)

# reminder, if something gets broken: default lock-byte for a mega32 is 0x3f

PROGRAMMER = -c avrispv2 -P usb -B 10
# PROGRAMMER contains AVRDUDE options to address your programmer
TARGET = usbasploader.elf

###############################################################################

# Tools:
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
CC = avr-gcc

# Options:
DEFINES = #-DDEBUG_LEVEL=2
#CFLAGS = -Wall -Os -I. -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) $(DEFINES)
#optimize:
CFLAGS = -Wall -Os -gdwarf-2 -I. -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) $(DEFINES)
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS)

OBJECTS =  usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o

# symbolic targets:
all: $(TARGET) usbasploader.hex usbasploader.lss size

#Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.S.o:
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(CC) $(CFLAGS) -S $< -o $@

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${DEVICE} ${TARGET}

flash:	all
	$(AVRDUDE) -U flash:w:usbasploader.hex:i

readflash:
	$(AVRDUDE) -U flash:r:read.hex:i

fuse:
	$(AVRDUDE) $(FUSEOPT)

lock:
	$(AVRDUDE) $(LOCKOPT)

read_fuses:
	$(UISP) --rd_fuses

clean:
	rm -f usbasploader.hex main.bin *.o usbdrv/*.o main.s usbdrv/oddebug.s usbdrv/usbdrv.s

# file targets:
main.bin:	$(OBJECTS)
	$(CC) $(CFLAGS) -o main.bin $(OBJECTS) $(LDFLAGS)

usbasploader.hex:	main.bin
	rm -f usbasploader.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin usbasploader.hex
	avr-size usbasploader.hex

disasm:	main.bin
	avr-objdump -d main.bin

cpp:
	$(CC) $(CFLAGS) -E main.c
