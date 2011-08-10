/*
    Copyright (c) 2001, Meldora Inc.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following
    conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
      in the documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
    OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
    OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

/*
	Contributors:
		Daniel Cliche (dcliche@meldora.com)
*/

char curx;
char cury;

#define KBD		0xE020
#define VDU		0xE030

/* Print a single character */
void printc(c)
char c;
{
	*(VDU + 3) = cury;
	*(VDU + 2) = curx;
	*(VDU) = c;
	curx++;
	*(VDU + 2) = curx;
}

/* Print a NULL-terminated string */
void prints(s)
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

/* Move the cursor to a new position */
void moveto(a, b)
char a;
char b;
{
	curx = a;
	cury = b;
	*(VDU + 2) = curx;
	*(VDU + 3) = cury;
}

/* Clear the screen */
void clearscr()
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
/* Print 4-bit hex */
printh4(d4)
unsigned d4;
{
	if (d4 > 9) {
		printc(d4 - 10 + 'A');
	} else {
		printc(d4 + '0');
	}
}

/* Print 8-bit hex */
void printh8(d8)
unsigned d8;
{
	printh4(d8 >> 4);
	printh4(d8 & 0xF);
}

/* Get a character */
char getch()
{
	/* Wait until the keyboard is ready */
	while (!(*(KBD) & 0x1));
	
	/* Return the key value */
	return *(KBD + 1);
}
