AS=../../tools/bin/as09

.PHONY: all

all: demo.s19

demo.s19: demo.asm
	${AS} $<

deploy:
	python ../../tools/deploy/deploy.py demo.s19

clean:
	rm -f *.s19
