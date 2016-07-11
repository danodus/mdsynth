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

/* Main */
int main(argc, argv)
int argc;
char **argv;
{
	unsigned m, lastm, p1, p2;
	
	clearscr();

	moveto(0, 0);
	prints("MIDI Test");


	/* We initialize the MIDI port */
	minit();

	moveto(0, 1);

	lastm = 0;

	m = mgetch();

	/* Main loop */
	while (1) {
		if (m < 0x80) {
			m = lastm;
		}

		m = m & 0xF0;

		if (m == 0x80) {
			p1 = mgetch();
			p2 = mgetch();
			prints("[NOTE OFF]");
		} else if (m == 0x90) {
			p1 = mgetch();
			p2 = mgetch();
			prints("[NOTE ON]");
		} else if (m == 0xA0) {
				/* Polyphonic key pressure */
			p1 = mgetch();
			p2 = mgetch();
			prints("[POLY KEY PRESSURE]");
		} else if (m == 0xB0) {
			/* Controller change */
			p1 = mgetch();
			p2 = mgetch();
			prints("[CTRL CHANGE]");
		} else if (m == 0xC0) {
			/* Program change */
			p1 = mgetch();
			prints("[PGRM CHANGE]");
		} else if (m == 0xD0) {
			/* Channel key pressure */
			p1 = mgetch();
			prints("[CHAN KEY PRESSURE]");
		} else if (m == 0xE0) {
			/* Pitch bend */
			p1 = mgetch();
			p2 = mgetch();
			prints("[PITCH BEND]");
		} else if (m == 0xF0) {
			prints(".");
		} else {
			prints("[*** ERROR: ");
			printh8(m);
			/* Synchronize */
			while(1) {
				m = mgetch();
				if (m < 0x80) {
					printh8(m);
				} else {
					break;
				}
			}
			prints(" ***]");
			continue;
		}
		lastm = m;
		m = mgetch();
	} /* while 1 */
	

	return 0;
}

