MMCU = atmega328p
FCPU = 16000000

CC = avr-gcc
FLAGS = -mmcu=$(MMCU)
CFLAGS = -Wall $(FLAGS) -DF_CPU=$(FCPU) -Os -I../libs
LDFLAGS = $(FLAGS) -Llibs -ldevice

TARGET = ordonnanceur

all: $(TARGET).hex

clean:
	rm -f *.o $(TARGET).hex $(TARGET).elf $(TARGET)

$(TARGET).o: $(TARGET).c $(TARGET).h
	$(CC) $(CFLAGS) -c -Wall -I. $< -o $@

$(TARGET).elf: $(TARGET).o
	$(CC) $(FLAGS) -g -lm -Wl,--gc-sections -o $@ $< $(LDFLAGS)

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -j .text -j .data -O ihex $< $@

upload: $(TARGET).hex
	stty -F /dev/ttyACM0 hupcl
	avrdude -F -v -p $(MMCU) -c stk500v1 -b 115200 -P /dev/ttyACM0 -U flash:w:$<
