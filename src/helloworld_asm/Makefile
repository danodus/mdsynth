AS=../../tools/bin/as09

.PHONY: all

all: hello.s19

hello.s19: hello.asm
	${AS} $<

clean:
	rm -f *.s19
