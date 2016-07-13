/*
MDSynth

Copyright (c) 2011-2016, Meldora Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
      following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

int	isnoteon;

/* Phase deltas */
unsigned phds[12];

/* Pitch stack */
char pstack[PSTACK_SIZE];
int psti;

/*
    pitch 69 (A4) will give the following: octave=6, note=9 
    The desired frequencies for octave 6 are the following:
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
	
	/*
	octave = pitch / 12;
	note = pitch % 12;
	*/

	if (pitch >= 12 && pitch < 24) {
		octave = 2;
		note = pitch - 12;
	} else if (pitch >= 24 && pitch < 36) {
		octave = 3;
		note = pitch - 24;
	} else  if (pitch >= 36 && pitch < 48) {
		octave = 4;
		note = pitch - 36;
	} else  if (pitch >= 48 && pitch < 60) {
		octave = 5;
		note = pitch - 48;
	} else  if (pitch >= 60 && pitch < 72) {
		octave = 6;
		note = pitch - 60;
	} else  if (pitch >= 72 && pitch < 84) {
		octave = 7;
		note = pitch - 72;
	} else  if (pitch >= 84 && pitch < 96) {
		octave = 8;
		note = pitch - 84;
	} else  if (pitch >= 96 && pitch < 108) {
		octave = 9;
		note = pitch - 96;
	} else {
		/* Note OFF */
		sndl[0] = 0x7f & waveform;
		sndr[0] = 0x7f & waveform;
		return;
	}
	
	phd = octave << 12;
	phd |= phds[note];	

	sndl[1] = phd >> 8;
	sndl[2] = phd;
	sndl[3] = octcarr + octave;
	
	sndl[4] = gainmsg;
	sndl[5] = gainmod;
	
	sndr[1] = phd >> 8;
	sndr[2] = phd;
	sndr[3] = octcarr + octave;
	
	sndr[4] = gainmsg;
	sndr[5] = gainmod;
	
	/* Note OFF */
	sndl[0] = 0x7f & waveform;
	sndr[0] = 0x7f & waveform;

	/* Note ON */
	sndl[0] = 0x80 | waveform;
	sndr[0] = 0x80 | waveform;
}

void noteon(pitch)
unsigned pitch;
{
	isnoteon = -1;
	play(pitch);
}

void noteoff()
{
	play(0);
	isnoteon = 0;
}

void prtstatu()
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

void prtstack()
{
	int i;
	moveto(0, 23);
	for (i = 0; i < PSTACK_SIZE; i++) {
		if (i <= psti) {
			printh8(pstack[i]);
		} else {
			prints("  ");
		}
	}
}

/* Main */
int main(argc, argv)
int argc;
char **argv;
{
	int wait;

	unsigned c, m, lastm, p1, p2, chan;
	char note;
	char lstpitch;
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
	waveform = 4;
	
	gainmsg = 63;
	gainmod = 63;

	isnoteon = 0;
	
	clearscr();

	moveto(0, 0);
	prints("MDSynth");

	moveto(0, 1);
	prints("Copyright (c) 2011-2016, Meldora Inc.");
	
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
	prints("y(t) = modulated_gain * sin(pi*m(t) + 2*pi*(octave+octave_carr)*freq*t)");
	moveto(10, 19);
	prints("where m(t) = message_gain * sin(2*pi*octave*freq*t)");
	
	pstack[0] = 0;
	psti = 0;
	
	note = -1;
	octave = 5;
	lstpitch = 0;

	/* We initialize the MIDI port */
	minit();

	lastm = 0;

	/* Main loop */
	while (1) {
	
		/* First, we check if we have an incoming MIDI note from the MIDI port */
		if (mcheckch()) {
			m = mgetch();
			if ((m & 0xF0) == 0xF0)
				continue;
			if (m < 0x80) {
				p1 = m;
				m = lastm;
			} else {
				lastm = m;
				p1 = mgetch();
			}
			chan = m & 0x0F;
			m = m & 0xF0;
			if (m == 0x90) {
				pitch = p1;
				p2 = mgetch();

				if (chan == 0) {
					if (psti < PSTACK_SIZE - 1) {
						psti++;
						pstack[psti] = pitch;

						if (pitch != lstpitch) {
							noteon(pitch);
							lstpitch = pitch;
						}
					}
				}
			} else if (m == 0x80) {
				p2 = mgetch();
				if (chan == 0) {
					for (i = 1; i < psti; i++)
						if (pstack[i] == p1) {
							for (; i < psti; i++)
								pstack[i] = pstack[i + 1];
							break;
						}
					if (psti > 0)
						psti--;
					pitch = pstack[psti];

					if (pitch > 0) {
						if (pitch != lstpitch) {
							noteon(pitch);
						}
					} else {
						noteoff();
					}
					lstpitch = pitch;
				}
			} else if (m == 0xA0) {
				/* Polyphonic key pressure */
				p2 = mgetch();
			} else if (m == 0xB0) {
				/* Controller change */
				p2 = mgetch();
			} else if (m == 0xC0) {
				/* Program change */
			} else if (m == 0xD0) {
				/* Channel key pressure */
			} else if (m == 0xE0) {
				/* Pitch bend */
				p2 = mgetch();
			}
			else {
				/* Unknown message */
				moveto(0, 22);
				printh8(m);
			}
			
			/* We print the pitch stack and status */
			prtstack();
			prtstatu();			
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
					note = -1;
					newpitch = 1;
					psti = 0;
					prtstack();
			}
			
			if (newpitch) {
				if (note >= 0) {
					pitch = 12 * octave + note;
					noteon(pitch);
				} else {
					pitch = 0;
					noteoff();
				}
			}
			
			prtstatu();
			
		} /* if a key is available */

	} /* while 1 */
	

	return 0;
}

