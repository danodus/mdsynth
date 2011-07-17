
#asm
#include "../common/c.asm"	
#endasm

#include "../common/io.c"

int main(argc, argv)
int argc;
char **argv;
{
	clearscr();
	curx = 0;
	cury = 0;
	prints("Hello World!");
	
	return 0;
}
