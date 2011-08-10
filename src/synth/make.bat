..\..\tools\Release\mc09.exe synth.c
type ..\common\c.asm > synth.asm
type c.out >> synth.asm
..\..\tools\Release\as09.exe synth.asm
