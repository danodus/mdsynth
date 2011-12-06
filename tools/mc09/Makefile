CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=mc09.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mc09

.PHONY: all clean

all: $(SOURCES) $(EXECUTABLE)
	mkdir -p ../bin
	cp $(EXECUTABLE) ../bin
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	rm -f ../bin/$(EXECUTABLE)

