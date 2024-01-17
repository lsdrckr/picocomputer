MMCU = atmega328p
FCPU = 16000000

CC = avr-gcc
FLAGS = -mmcu=$(MMCU)
CFLAGS = -Wall $(FLAGS) -DF_CPU=$(FCPU) -Os -Ilibs
LDFLAGS = $(FLAGS) -Llibs -ldevice

TARGET = ordonnanceur

all: libs $(TARGET).hex

libdevice.a: 
	$(MAKE) -C libs

clean:
	make -C libs clean
	rm -f *.o $(TARGET).hex $(TARGET).elf $(TARGET)

$(TARGET).o: $(TARGET).c $(TARGET).h libdevice.a
	$(CC) $(CFLAGS) -c -Wall -I. $< -o $@

$(TARGET).elf: $(TARGET).o
	$(CC) $(FLAGS) -g -lm -Wl,--gc-sections -o $@ $< $(LDFLAGS)

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -j .text -j .data -O ihex $< $@

upload: all
	stty -F /dev/ttyACM0 hupcl
	avrdude -F -v -p $(MMCU) -c stk500v1 -b 115200 -P /dev/ttyACM0 -U flash:w:$(TARGET).hex
