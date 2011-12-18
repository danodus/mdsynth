AS=../../tools/bin/as09
CC=../../tools/bin/mc09

.PHONY: all

all: miditest.s19

miditest.s19: miditest.asm
	${AS} $<

miditest.asm: ../common/c.asm c.out
	cat ../common/c.asm > $@
	cat c.out >> $@

c.out: miditest.c
	${CC} $<

deploy:
	python ../../tools/deploy/deploy.py miditest.s19

clean:
	rm -f miditest.asm c.out *.s19
