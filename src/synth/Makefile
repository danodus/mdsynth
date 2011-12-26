AS=../../tools/bin/as09
CC=../../tools/bin/mc09

.PHONY: all

all: synth.s19

synth.s19: synth.asm
	${AS} $<

synth.asm: ../common/c.asm c.out
	cat ../common/c.asm > $@
	cat c.out >> $@

c.out: synth.c
	${CC} -s $<

deploy:
	python ../../tools/deploy/deploy.py synth.s19

clean:
	rm -f synth.asm c.out *.s19
