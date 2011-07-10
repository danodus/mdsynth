
MKFRAGS := ../../mkfiles
export MKFRAGS

C_SRC := as.c as09.c do09.c eval.c ffwd.c ifd.c output.c pseudo.c symtab.c util.c
HEADERS := as.h table09.h

include $(MKFRAGS)/def_cmds.mk

as09$(EXE_EXT): $(HEADERS) $(C_SRC)
	@$(ECHO)
	@$(ECHO) "======= Making 6809 assembler ================"
	gcc -I. -DWIN32 -o $@ as09.c

clean:
	rm as09$(EXE_EXT)


