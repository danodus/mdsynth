..\..\tools\Release\as09.exe sys09mds.asm > listing.txt
..\..\tools\Release\s19tovhd.exe b16 sys09mds.s19 mon_rom_vhd SYS09BUG F800
rem copy mon_rom_vhd ..\..\rtl\src\sys09bug_mds_rom2k_b16.vhd
