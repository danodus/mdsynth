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

/* Sound chip with 4 voices */
#define SND0		0xE080
#define SND1		0xE090
#define SND2		0xE0A0
#define SND3		0xE0B0

unsigned waveform;
char octave;		/* keyboard main octave */
char oct;		/* sound chip octave */
char octcarr;		/* carrier octave */
unsigned gainmsg;	/* message gain */
unsigned gainmod;	/* modulated gain */
unsigned wavmsg;	/* message waveform */
unsigned wavmod; 	/* modulated waveform */
unsigned attrate;	/* attack rate */
unsigned relrate;	/* release rate */

char plist[4];
unsigned alist[4];
unsigned age;

/* Phase deltas */
unsigned phds[12];

/* Voices */
char *voices[4];

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

void sndupdt(voice)
int voice;
{
	char *snd;
	snd = voices[voice];
	snd[3] = octcarr + oct;
	snd[4] = gainmsg | (wavmsg << 6);
	snd[5] = gainmod | (wavmod << 6);
	snd[7] = (relrate << 4) | attrate; 
}

void play(voice, pitch)
int voice;
unsigned pitch;
{
	unsigned note;
	unsigned phd;
	unsigned o;
	char *snd;
	snd = voices[voice];

	/*
	octave = pitch / 12;
	note = pitch % 12;
	*/

	if (pitch >= 12 && pitch < 24) {
		oct = 2;
		note = pitch - 12;
	} else if (pitch >= 24 && pitch < 36) {
		oct = 3;
		note = pitch - 24;
	} else  if (pitch >= 36 && pitch < 48) {
		oct = 4;
		note = pitch - 36;
	} else  if (pitch >= 48 && pitch < 60) {
		oct = 5;
		note = pitch - 48;
	} else  if (pitch >= 60 && pitch < 72) {
		oct = 6;
		note = pitch - 60;
	} else  if (pitch >= 72 && pitch < 84) {
		oct = 7;
		note = pitch - 72;
	} else  if (pitch >= 84 && pitch < 96) {
		oct = 8;
		note = pitch - 84;
	} else  if (pitch >= 96 && pitch < 108) {
		oct = 9;
		note = pitch - 96;
	} else {
		/* Note OFF */
		if (waveform == 4) {
			snd[0] = 0x7f & waveform;
		} else {
			snd[0] = 0;
		}
		return;
	}
	
	o = oct;
	phd = o << 12;
	phd |= phds[note];	

	snd[1] = phd >> 8;
	snd[2] = phd;
		
	sndupdt(voice);
	
	/* Note ON */
	snd[0] = 0x80 | waveform;
}

void noteon(voice, pitch)
int voice;
unsigned pitch;
{
	play(voice, pitch);
}

void noteoff(voice)
{
	play(voice, 0);
}

void prtstatu()
{
	moveto(16, 12);
	printh4(octave);
	moveto(16, 13);
	printh4(octcarr);
	moveto(26, 14);
	printh8(gainmsg);			
	moveto(26, 15);
	printh8(gainmod);
	moveto(32, 14);
	printh8(wavmsg);			
	moveto(32, 15);
	printh8(wavmod);
	moveto(32, 16);
	printh8(attrate);
	moveto(36, 16);
	printh8(relrate);
	moveto(50, 16);
	printh8(voices[0][0]);
	moveto(54, 16);
	printh8(voices[1][0]);
	moveto(58, 16);
	printh8(voices[2][0]);
	moveto(62, 16);
	printh8(voices[3][0]);
}


/* Main */
int main(argc, argv)
int argc;
char **argv;
{
	unsigned c, m, lastm, p1, p2, chan;
	char note;
	unsigned minage;
	char pitch;
	int i, i2;
	int newpitch;
	unsigned env;

	voices[0] = SND0;
	voices[1] = SND1;
	voices[2] = SND2;
	voices[3] = SND3;

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
	wavmsg = 0;
	wavmod = 0;
	attrate = 15;
	relrate = 15;

	plist[0] = 0;
	plist[1] = 0;
	plist[2] = 0;
	plist[3] = 0;

	alist[0] = 0;
	alist[1] = 0;
	alist[2] = 0;
	alist[3] = 0;

	age = 0;
	
	clearscr();

	hidecsr();

	moveto(0, 0);
	prints("MDSynth Poly");

	moveto(0, 1);
	prints("Copyright (c) 2011-2017, Meldora Inc.");
	
	moveto(10, 3);
	prints(" W E   T Y U");
	moveto(10, 4);
	prints("A S D F G H J      Spacebar: Note OFF");
	moveto(10, 5);
	prints("z, Z: Octave +/-");
	moveto(10, 6);
	prints("x, X: Relative Octave Carrier +/- (PM only)");
	moveto(10, 7);
	prints("c, C: Message Gain +/- (Sine and PM only)");
	moveto(10, 8);
	prints("v, V: Modulation Gain +/- (PM only)");
	moveto(10, 9);
	prints("b, B: Attack Rate +/- (PM only)");
	moveto(10, 10);
	prints("n, N: Release Rate +/- (PM only)");
	moveto(10, 11);
	prints("1, 2, 3, 4: Square/Saw/Sine/PM, 5, 6: Message/modulated waveform");
	
	moveto(0, 12);
	prints("Octave:");
	moveto(0, 13);
	prints("Octave Carrier:");	
	moveto(0, 14);
	prints("Message Gain, Waveform:");
	moveto(0, 15);
	prints("Modulated Gain, Waveform:");
	moveto(0, 16);
	prints("Attack/Release Rate:");

	moveto(0, 17);
	prints("Phase Modulation:");
	moveto(4, 18);
	prints("y(t) = modulated_gain * sin(pi*m(t) + 2*pi*(octave+octave_carr)*freq*t)");
	moveto(4, 19);
	prints("where m(t) = message_gain * sin(2*pi*octave*freq*t)");
		
	note = -1;
	octave = 5;

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
					for (i = 0; i < 4; ++i) {
						if (!plist[i]) {
							/* If the envelope phase is idle */
							env = voices[i][0];
							if ((env & 0xFF) == 0x00) {
								alist[i] = age;
								plist[i] = pitch;
								noteon(i, pitch);
								age++;
								break;
							}
						}
					}
					/* Steal a note if necessary */
					if (i == 4) {
						i2 = 0;
						minage = 65535;
						for (i = 0; i < 4; ++i) {
							if (alist[i] < minage) {
								minage = alist[i];
								i2 = i;
							}
						}
						alist[i2] = age;
						plist[i2] = pitch;
						noteon(i2, pitch);
						age++;
					}
				}
			} else if (m == 0x80) {
				p2 = mgetch();
				if (chan == 0) {
					for (i = 0; i < 4; ++i)
						if (plist[i] == p1) {
							noteoff(i);
							plist[i] = 0;
							break;
						}
				}
			} else if (m == 0xA0) {
				/* Polyphonic key pressure */
				p2 = mgetch();
			} else if (m == 0xB0) {
				/* Controller change */
				p2 = mgetch();
				if (p1 == 45) {
					gainmsg = p2 >> 1;
				} else if (p1 == 46) {
					octcarr = p2 >> 3;
				} else if (p1 == 47) {
					wavmsg = p2 >> 5;
				} else if (p1 == 5) {
					wavmod = p2 >> 5;
				}
				sndupdt(0);
				sndupdt(1);
				sndupdt(2);
				sndupdt(3);

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
			/*prtstatu();*/			
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
				
				case 'Z':
				if (octave > 0) {
					octave--;
					newpitch = 1;
				}
				break;
				
				case 'z':
				if (octave < 9) {
					octave++;
					newpitch = 1;
				}
				break;
				
				case '1':
				waveform = 1;
				newpitch = 1;
				break;
				
				case '2':
				waveform = 2;
				newpitch = 1;
				break;
				
				case '3':
				waveform = 3;
				newpitch = 1;
				break;
				
				case '4':
				waveform = 4;
				newpitch = 1;
				break;

				case 'X':
				if (octcarr > -8)
					octcarr--;
				break;
				
				case 'x':
				if (octcarr < 7)
					octcarr++;
				break;
								
				case 'C':
				if (gainmsg > 0)
					gainmsg--;
				break;
				
				case 'c':
				if (gainmsg < 63)
					gainmsg++;
				break;
				
				case 'V':
				if (gainmod > 0)
					gainmod--;
				break;
				
				case 'v':
				if (gainmod < 63)
					gainmod++;
				break;

				case 'B':
				if (attrate > 0)
					attrate--;
				break;

				case 'b':
				if (attrate < 15)
					attrate++;
				break;

				case 'N':
				if (relrate > 0)
					relrate--;
				break;

				case 'n':
				if (relrate < 15)
					relrate++;
				break;

				case '5':
				wavmsg = (wavmsg + 1) % 4;
				break;
				
				case '6':
				wavmod = (wavmod + 1) % 4;
				break;

				case ' ':
					note = -1;
					newpitch = 1;
			}
			
			if (newpitch) {
				if (note >= 0) {
					pitch = 12 * octave + note;
					noteon(0, pitch);
				} else {
					pitch = 0;
					noteoff(0);
				}
			} else {
				sndupdt(0);
				sndupdt(1);
				sndupdt(2);
				sndupdt(3);
			}
			
			/*prtstatu();*/
			
		} /* if a key is available */

		prtstatu();

	} /* while 1 */
	

	return 0;
}

