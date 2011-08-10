..\..\tools\Debug\mc09.exe test.c
type ..\common\c.asm > test.asm
type c.out >> test.asm
..\..\tools\Debug\as09.exe test.asm > listing.txt

