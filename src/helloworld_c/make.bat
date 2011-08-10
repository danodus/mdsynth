..\..\tools\Release\mc09.exe hello.c
type ..\common\c.asm > hello.asm
type c.out >> hello.asm
..\..\tools\Release\as09.exe hello.asm > listing.txt

