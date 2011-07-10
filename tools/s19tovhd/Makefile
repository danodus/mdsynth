
MKFRAGS := ../../mkfiles
export MKFRAGS

C_SRC := S19toVHD.cpp
HEADERS := 

include $(MKFRAGS)/def_cmds.mk

S19toVHD$(EXE_EXT): $(C_SRC) $(HEADERS)
	@$(ECHO)
	@$(ECHO) "======= Making S19 to VHDL rom code generator ================"
	g++ -I. -DWIN32 -o $@ $<

clean:
	-rm -f S19toVHD$(EXE_EXT)


