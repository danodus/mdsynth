..\..\tools\Release\mc09.exe miditest.c
pause
type ..\common\c.asm > miditest.asm
type c.out >> miditest.asm
..\..\tools\Release\as09.exe miditest.asm
