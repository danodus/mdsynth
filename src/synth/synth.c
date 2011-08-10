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

char *sndl;
char *sndr;

unsigned waveform;
unsigned carrphd;	/* carrier phase delta */

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
		sndl[3] = carrphd >> 8;
		sndl[4] = carrphd;
		sndl[0] = waveform;
		
		sndr[1] = phd >> 8;
		sndr[2] = phd;
		sndr[3] = carrphd >> 8;
		sndr[4] = carrphd;
		sndr[0] = waveform;
	} else {
		/* Turn off the oscillators */
		sndl[0] = 0;
		sndr[0] = 0;
	}
}

/* Main */
int main(argc, argv)
int argc;
char **argv;
{
	char c;
	char note;
	char octave;
	char pitch;
	int quiet;

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

	carrphd = 1024;
	waveform = 1;
	
	clearscr();

	moveto(0, 0);
	prints("MDSynth");

	moveto(0, 1);
	prints("Copyright (c) 2011, Meldora Inc.");
	
	moveto(10, 3);
	prints(" W E R   Y U");
	moveto(10, 4);
	prints("A S D F G H J");
	moveto(10, 5);
	prints("Z, X: Octave +/-   1, 2, 3, 4: Square/Tri/Sine/PhMod");
	moveto(10, 6);
	prints("C, V: PhMod Carrier PhD +/-     Any other key: Quiet");

	moveto(0, 9);
	prints("Octave:");
	moveto(0, 10);
	prints("CarrPhD:");
	moveto(0, 11);
	prints("Pitch:");
	
	octave = 5;
	while (1) {
		c = getch();
		switch (c) {
			case 'a':
			case 'A':
			note = 0;			
			break;

			case 'w':
			case 'W':
			note = 1;
			break;

			case 's':
			case 'S':
			note = 2;
			break;

			case 'E':
			case 'e':
			note = 3;
			break;

			case 'd':
			case 'D':
			note = 4;
			break;

			case 'r':
			case 'R':
			note = 5;
			break;
			
			case 'f':
			case 'F':
			note = 6;
			break;
			
			case 'g':
			case 'G':
			note = 7;
			break;
			
			case 'y':
			case 'Y':
			note = 8;
			break;
			
			case 'h':
			case 'H':
			note = 9;
			break;
			
			case 'u':
			case 'U':
			note = 10;
			break;
			
			case 'j':
			case 'J':
			note = 11;
			break;
			
			case 'z':
			case 'Z':
			if (octave > 0)
				octave--;
			break;
			
			case 'x':
			case 'X':
			if (octave < 9)
				octave++;
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
			carrphd-=8;
			break;

			case 'C':
			carrphd-=128;
			break;
			
			case 'v':
			carrphd+=8;
			break;
			
			case 'V':
			carrphd+=128;
			break;

			default:
			note = -1;
		}
		
		moveto(10, 9);
		printh8(octave);
		moveto(10, 10);
		printh8(carrphd >> 8);
		printh8(carrphd & 0xff);

		if (note >= 0) {
			pitch = 12 * octave + note;
			moveto(10, 11);
			printh8(pitch);
			play(pitch);
		} else {
			moveto(10, 11);
			prints("--");
			play(0);
		}
	}

	return 0;
}
