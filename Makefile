CC = gcc
CFLAGS = -Wall -O2 -std=c99
LDFLAGS = -lm

SOURCES = main.c pgm_utils.c scan_utils.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = scan_reconstruction.exe

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q *.o $(EXECUTABLE) 2>nul || true

.PHONY: all clean
