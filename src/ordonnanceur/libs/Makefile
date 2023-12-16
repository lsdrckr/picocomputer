CC = avr-gcc
MCU = atmega328p
F_CPU = 16000000
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -I../

LIBRARY = libdevice.a

SRCS = device.c keyboard.c serial.c
OBJS = $(SRCS:.c=.o)
HEADERS = device.h keyboard.h serial.h

.PHONY: all clean

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	avr-ar rcs $(LIBRARY) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(LIBRARY)