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

#include "../common/io.c"

/* Sound chip */
#define SNDL	0xE080
#define SNDR	0xE090

#define PSTACK_SIZE	16

char *sndl;
char *sndr;

unsigned waveform;
char octave;		/* main octave */
char octcarr;		/* carrier octave */
unsigned gainmsg;	/* message gain */
unsigned gainmod;	/* modulated gain */

/* Phase deltas */
unsigned phds[12];

/*
    pitch 69 (A4) will give the following: octave=5, note=9 
    The desired frequencies for octave 5 are the following:
        note     freq (Hz)
        0        261.63    (C4)
        1        277.18
        2        293.66
        3        311.13
        4        329.63
        5        349.23
        6        369.99
        7        392.00
        8        415.30
        9        440.00    (A4)
        10       466.16
        11       493.88
    
    The frequency given to NCO is the following:
		freq = (50E6 * (phase_delta * 2^octave)) / 2^32
		phase_delta = freq * 2^32 / (50E6 * 2^octave)
*/

void play(pitch)
unsigned pitch;
{
	unsigned octave;
	unsigned note;
	unsigned phd;
	
	if (pitch) {
		octave = pitch / 12;
		note = pitch % 12;
		
		phd = octave << 12;
		phd |= phds[note];	

		sndl[1] = phd >> 8;
		sndl[2] = phd;
		sndl[3] = octcarr + octave;
		
		sndl[4] = gainmsg;
		sndl[5] = gainmod;
		
		sndl[0] = waveform;
		
		sndr[1] = phd >> 8;
		sndr[2] = phd;
		sndr[3] = octcarr + octave;
		
		sndr[4] = gainmsg;
		sndr[5] = gainmod;
		
		sndr[0] = waveform;
	} else {
		/* Turn off the oscillators */
		sndl[0] = 0;
		sndr[0] = 0;
	}
}

void prtstatus()
{
	moveto(16, 12);
	printh4(octave);
	moveto(16, 13);
	printh4(octcarr);
	moveto(16, 14);
	printh8(gainmsg);			
	moveto(16, 15);
	printh8(gainmod);
}

/* Main */
int main(argc, argv)
int argc;
char **argv;
{
	char pstack[PSTACK_SIZE];
	int psti;
	
	unsigned c;
	char note;
	char pitch;
	int i;
	int newpitch;

	sndl = SNDL;
	sndr = SNDR;

	phds[0] = 702;
	phds[1] = 744;
	phds[2] = 788;
	phds[3] = 835;
	phds[4] = 885;
	phds[5] = 937;
	phds[6] = 993;
	phds[7] = 1052;
	phds[8] = 1115;
	phds[9] = 1181;
	phds[10] = 1251;
	phds[11] = 1326;

	octcarr = 0;
	waveform = 1;
	
	gainmsg = 63;
	gainmod = 63;
	
	clearscr();

	moveto(0, 0);
	prints("MDSynth");

	moveto(0, 1);
	prints("Copyright (c) 2011, Meldora Inc.");
	
	moveto(10, 3);
	prints(" W E   T Y U");
	moveto(10, 4);
	prints("A S D F G H J");
	moveto(10, 5);
	prints("Z, X: Octave +/-");
	moveto(10, 6);
	prints("C, V: Relative Octave Carrier +/- (PM only)");
	moveto(10, 7);
	prints("B, N: Message Gain +/- (Sine and PM only)");
	moveto(10, 8);
	prints("M, ',': Modulation Gain +/- (PM only)");
	moveto(10, 9);
	prints("1, 2, 3, 4: Square/Saw/Sine/PM");
	moveto(10, 10);
	prints("Spacebar: Quiet");
	
	moveto(0, 12);
	prints("Octave:");
	moveto(0, 13);
	prints("Octave Carrier:");	
	moveto(0, 14);
	prints("Message Gain:");
	moveto(0, 15);
	prints("Modulated Gain:");

	moveto(0, 17);
	prints("Phase Modulation:");
	moveto(10, 18);
	prints("y(t) = modulated_gain * sin(m(t) + 2*pi*(octave+octave_carr)*freq*t)");
	moveto(10, 19);
	prints("where m(t) = message_gain * sin(2*pi*octave*freq*t)");
	
	pstack[0] = 0;
	psti = 0;
	
	note = -1;
	octave = 5;
	while (1) {
	
		/* First, we check if we have an incoming MIDI note from the serial port */
		if (scheckch()) {
			c = sgetch();
			if (c == 0x90) {
				c = sgetch();
				if (psti < PSTACK_SIZE - 1) {
					psti++;
					pitch = c;
					pstack[psti] = pitch;
				}
				c = sgetch();
			} else if (c == 0x80) {
				c = sgetch();
				for (i = 1; i < psti; i++)
					if (pstack[i] == c) {
						for (; i < psti; i++)
							pstack[i] = pstack[i + 1];
						break;
					}
				if (psti > 0)
					psti--;
				pitch = pstack[psti];
				c = sgetch();
			} else if (c == 0xB0) {
				c = sgetch();
				if (c == 0x17) {
					c = sgetch();
					waveform = c >> 4;
				} else if (c == 0x18) {
					c = sgetch();
					octcarr = c >> 4;
				} else if (c == 0x19) {
					c = sgetch();
					gainmsg = c >> 1;
				} else if (c == 0x1A) {
					c = sgetch();
					gainmod = c >> 1;
				}				
				
			}
			
			/* We print the pitch stack */
			moveto(0, 23);
			for (i = 0; i < PSTACK_SIZE; i++) {
				if (i <= psti) {
					printh8(pstack[i]);
				} else {
					prints("  ");
				}
			}
			
			prtstatus();			
			play(pitch);
		}
	
		/* We check if we have an incoming key */
		if (checkch()) {
			c = getch();
			newpitch = 0;
			switch (c) {
				case 'a':
				case 'A':
				note = 0;
				newpitch = 1;
				break;

				case 'w':
				case 'W':
				note = 1;
				newpitch = 1;
				break;

				case 's':
				case 'S':
				note = 2;
				newpitch = 1;
				break;

				case 'E':
				case 'e':
				note = 3;
				newpitch = 1;
				break;

				case 'd':
				case 'D':
				note = 4;
				newpitch = 1;
				break;

				case 'f':
				case 'F':
				note = 5;
				newpitch = 1;
				break;
				
				case 't':
				case 'T':
				note = 6;
				newpitch = 1;
				break;
				
				case 'g':
				case 'G':
				note = 7;
				newpitch = 1;
				break;
				
				case 'y':
				case 'Y':
				note = 8;
				newpitch = 1;
				break;
				
				case 'h':
				case 'H':
				note = 9;
				newpitch = 1;
				break;
				
				case 'u':
				case 'U':
				note = 10;
				newpitch = 1;
				break;
				
				case 'j':
				case 'J':
				note = 11;
				newpitch = 1;
				break;
				
				case 'z':
				case 'Z':
				if (octave > 0) {
					octave--;
					newpitch = 1;
				}
				break;
				
				case 'x':
				case 'X':
				if (octave < 9) {
					octave++;
					newpitch = 1;
				}
				break;
				
				case '1':
				waveform = 1;
				break;
				
				case '2':
				waveform = 2;
				break;
				
				case '3':
				waveform = 3;
				break;
				
				case '4':
				waveform = 4;
				break;

				case 'c':
				case 'C':
				if (octcarr > -8)
					octcarr--;
				break;
				
				case 'v':
				case 'V':
				if (octcarr < 7)
					octcarr++;
				break;
								
				case 'b':
				case 'B':
				if (gainmsg > 0)
					gainmsg--;
				break;
				
				case 'n':
				case 'N':
				if (gainmsg < 63)
					gainmsg++;
				break;
				
				case 'm':
				case 'M':
				if (gainmod > 0)
					gainmod--;
				break;
				
				case ',':
				case '<':
				if (gainmod < 63)
					gainmod++;
				break;
				
				case ' ':
					note = -1; /* quiet */
					newpitch = 1;
			} /* switch */
			
			if (newpitch) {
				if (note >= 0) {
					pitch = 12 * octave + note;
				} else {
					pitch = 0;
				}
			}
			
			prtstatus();
			play(pitch);
			
		} /* if a key is available */

	} /* while 1 */
	

	return 0;
}

