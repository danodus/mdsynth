CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=as09.c as.c do09.c eval.c ffwd.c ifd.c output.c pseudo.c symtab.c util.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=as09

.PHONY: all clean

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	mkdir -p ../bin
	cp $(EXECUTABLE) ../bin

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	rm -f ../bin/$(EXECUTABLE)


