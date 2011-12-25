AS=../../tools/bin/as09

.PHONY: all

all: irqtest.s19

irqtest.s19: irqtest.asm
	${AS} $<

clean:
	rm -f *.s19
