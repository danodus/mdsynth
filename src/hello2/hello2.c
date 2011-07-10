
#asm

#include "c.txt"	

#endasm

/**********************************/

char curx, cury;

#define KBD		0xE020
#define VDU		0xE030
#define SNDL	0xE080
#define SNDR	0xE090

/* Print a single character */
printc(c)
char c;
{
	*(VDU + 3) = cury;
	*(VDU + 2) = curx;
	*(VDU) = c;
	curx++;
	*(VDU + 2) = curx;
}

/* Print a NULL-terminated string */
prints(s)
char *s;
{
	*(VDU + 3) = cury;
	
	while (*s) {
		*(VDU + 2) = curx;
		*VDU = *s;
		curx++;
		s++;
	}
	
	*(VDU + 2) = curx;
}



clearscr()
{
	char x, y;
	for (y = 0; y < 25; y++) {
		*(VDU + 3) = y;
		for (x = 0; x < 80; x++) {
			*(VDU + 2) = x;
			*(VDU) = ' ';
		}
	}
}

printh4(d4)
unsigned d4;
{
	if (d4 > 9) {
		printc(d4 - 10 + 'A');
	} else {
		printc(d4 + '0');
	}
}

printh8(d8)
unsigned d8;
{
	printh4(d8 >> 4);
	printh4(d8 & 0xF);
}

char getch()
{
	/* Wait until the keyboard is ready */
	while (!(*(KBD) & 0x1));
	
	/* Return the key value */
	return *(KBD + 1);
}


int main(argc, argv)
int argc;
char **argv;
{
	char c;

	c = 0;
	curx = 0;
	cury = 0;
	
	clearscr();
	prints("Hello World!");
	prints(" -- Press a key");
	
	cury = 1;
		
	while(1) {
		c = getch();
		curx = 0;
		printh8(c);
	}
	
	return 0;
}
