..\..\tools\Release\mc09.exe synth.c
pause
type ..\common\c.asm > synth.asm
type c.out >> synth.asm
..\..\tools\Release\as09.exe synth.asm
