AS=../../tools/bin/as09
CC=../../tools/bin/mc09

.PHONY: all

all: hello.s19

hello.s19: hello.asm
	${AS} $<

hello.asm: ../common/c.asm c.out
	cat ../common/c.asm > $@
	cat c.out >> $@

c.out: hello.c
	${CC} $<



clean:
	rm -f hello.asm c.out *.s19
