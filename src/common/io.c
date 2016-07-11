/*
    Copyright (c) 2011, Meldora Inc.
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

#define ACIA		0xE000
#define MIDI_ACIA	0xE010
#define KBD		0xE020
#define VDU		0xE030

#define IRQ		0x7FC8

/* Print a single character */
void printc(c)
char c;
{
	*(VDU + 3) = cury;
	*(VDU + 2) = curx;
	*(VDU) = c;
	curx++;
	if (curx > 80) {
		curx = 0;
		cury++;
	}
	if (cury > 24)
		cury = 0;
	*(VDU + 3) = cury;
	*(VDU + 2) = curx;
}

/* Print a NULL-terminated string */
void prints(s)
char *s;
{
	while (*s) {
		printc(*s);
		s++;
	}
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
	d4 &= 0xF;
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
	d8 &= 0xFF;
	printh4(d8 >> 4);
	printh4(d8);
}

/* Print 16-bit hex */
void printh16(d16)
unsigned d16;
{
	printh8(d16 >> 8);
	printh8(d16);
}

/* Check if a character from the PS/2 keyboard is available */
int checkch()
{
	/* Check if the keyboard is ready */
	if (*(KBD) & 0x1)
		return -1;

	return 0;
}

/* Get a character from the PS/2 keyboard */
unsigned getch()
{
	unsigned c;
	
	/* Wait until the keyboard is ready */
	while (!(*(KBD) & 0x1));
	
	/* Return the key value */
	c = *(KBD + 1);
	return c & 0xFF;
}

/* Check if a character from the serial port is available */
int scheckch()
{
	/* Check if the ACIA is ready */
	if (*(ACIA) & 0x1)
		return -1;
		
	return 0;
}

/* Get a character from the serial port */
unsigned sgetch()
{
	unsigned c;
	
	/* Wait until the the ACIA is ready */
	while (!(*(ACIA) & 0x1));
	
	/* Return the character value */
	c = *(ACIA + 1);
	return c & 0xFF;
}

/* --------------- MIDI Support -------------------*/

#define MBUF_SIZE	16	/* MIDI buffer size */

char mbuf[MBUF_SIZE];	/* MIDI buffer */
unsigned mbufwi;	/* MIDI buffer read index */
unsigned mbufri;	/* MIDI buffer write index */
unsigned mbufc;		/* Number of characters in the MIDI buffer */

/* Check if a character from the MIDI port is available */
int mcheckch()
{
	if (mbufc > 0)
		return -1;

	return 0;
}

/* Get a character from the MIDI port */
unsigned mgetch()
{
	unsigned c;
	
	while (mbufc == 0);

	c = mbuf[mbufri];
	mbufri++;
	if (mbufri >= MBUF_SIZE)
		mbufri = 0;
	mbufc--;
			
	return c & 0xFF;
}

/* MIDI interrupt handler */
void mirqh()
{
	unsigned c;

	if (*(MIDI_ACIA) & 0x80) { 
		c = *(MIDI_ACIA + 1);
		if (mbufc < MBUF_SIZE) {
			mbuf[mbufwi] = c;
			mbufwi++;
			if (mbufwi >= MBUF_SIZE)
				mbufwi = 0;
			mbufc++;
		}
	}
}

#asm
MACIA	EQU	$E010
IRQ	EQU	$7FC8

IRQH	LBSR	mirqh
	RTI

* Initialize the MIDI port with interrupts

_minit

* We set the interrupt handler
	LDX	#IRQ
	LDD	#IRQH
	STD	,X

* We configure the MIDI ACIA
	LDX	#MACIA
	LDA	#$D5
	STA	,X

* We enable the interrupts
	ANDCC	#$EF

	RTS
#endasm

void minit()
{
	mbufwi = 0;
	mbufri = 0;
	mbufc = 0;
	
	_minit();
}

