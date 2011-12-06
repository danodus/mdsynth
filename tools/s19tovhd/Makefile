CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=s19tovhd.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=s19tovhd

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

