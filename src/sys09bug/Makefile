AS=../../tools/bin/as09
SV=../../tools/bin/s19tovhd

.PHONY: all

all: mon_rom_vhd
	@echo "Please copy the following code: mon_rom_vhd --> ..\..\rtl\src\sys09bug_mds_rom2k_b16.vhd"

mon_rom_vhd: sys09mds.s19
	${SV} b16 $< mon_rom_vhd SYS09BUG F800

sys09mds.s19: sys09mds.asm sys09bug.asm sys09equ.asm
	${AS} sys09mds.asm

clean:
	rm -f *.s19 mon_rom_vhd
