AS=../../tools/bin/as09

.PHONY: all

all: dacdirect.s19

dacdirect.s19: dacdirect.asm
	${AS} $<

deploy:
	python ../../tools/deploy/deploy.py dacdirect.s19

clean:
	rm -f *.s19
