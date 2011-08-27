MDSynth
FPGA-based synthesizer system for the Xilinx Spartan-3A and Spartan-3E Starter Kits

Author: Daniel Cliche (dcliche@meldora.com)
Copyright (c) 2011, Meldora Inc. All rights reserved.

License: New BSD for the sound synthesizer chip *only*, GPL for the complete 6809-based synthesizer system.
Languages: VHDL, 6809 assembly, C

============================================================
1. Overview
============================================================

The sound chip is a sigma-delta DAC and a numerically-controlled oscillator. It integrates a 6809-based system with keyboard support and video display output. The conversion from MIDI pitch to phase delta per clock with octave value is done by the 6809-based system for flexibility.
Four types of waveform are available:
	- Square (fixed gain)
	- Sawtooth (fixed gain)
	- Sine (variable gain)
	- Phase modulation (variable gain, similar to frequency modulation but easier to implement)

In phase modulation, the gains for both oscillators are applied without multiplications with the help of logarithmic and exponential tables. The technique is described below.
	
============================================================
2. Complete synthesizer as a 6809-based system
============================================================

Adaptation of the excellent System09 project by John E. Kent (dilbert57@opencores.org)
http://members.optushome.com.au/jekent/system09/


2.1 6809 Toolset
================

All the source files of the toolset are provided. The following is provided:

- A 6809 assembler (tools/as09);
- A C compiler (tools/mc09);
- S19 to VHDL (tools/s19tovhd) for producing the ROM file.

The toolset executables are included for Windows under the Release directory. A Microsoft Visual C++ 2008 Express solution is also included.

The S19 file is to be uploaded to the board using a serial terminal at 57600-N-8-1 (see the installation section). The ROM contains the SYS09BUG utility available under "src/sys09bug" which perform the download of the file over the serial port.

The chain is the following:
	"C" -- [ C compiler ] --> "ASM" -- [ 6809 assembler ] --> "S19" -- [ Serial upload with SYS09BUG ROM ] --> FPGA Block RAM Memory

The C compiler based on the public domain Micro-C is tiny (around 3K lines of code in a single file) and is quite impressive for its size (something I tend to appreciate when working with a 8-bit CPU!). I like the idea of having a small compiler easily adaptable for other processors. Obviously, no optimization is done and everything is done in a single pass, so there is no separation between data and code. It also only implement a subset of the C language. 

If you plan to use this tiny C compiler, please read the README file located the project for known limitations and issues I encountered.
	
2.2. Sound Chip Addresses
=========================
	
Two sound chips are available at the following base addresses:
- 0xE080	Left Audio
- 0xE090	Right Audio

Base + 0: Waveform (0x00: None, 0x01: Square, 0x02: Sawtooth, 0x03: Sine, 0x04: Phase Modulation)
Base + 1: Message Octave (4-bit) + Phase Delta MSB (4-bit)
Base + 2: Message Phase Delta LSB (8-bit)
Base + 3: Carrier Octave (4-bit) + Phase Delta MSB (4-bit)
Base + 4: Carrier Phase Delta LSB (8-bit)
Base + 5: Message Gain (6-bit, 63=1.0, sine and phase modulation only)
Base + 6: Modulated Gain (6-bit, 63=1.0, phase modulation only)

Refer to section 4.1 for more details about the 12-bit phase delta value to provide to the NCO along with the 4-bit octave value.

2.3. Installation
=================

1. Open a command prompt, go the the "src/synth" directory and execute "make.bat". This command will produce the "synth.s19" file to upload.
2. Connect a VGA monitor to the synthesizer (your Xilinx Spartan 3A-Starter Kit board);
3. Connect a PS/2 keyboard to the synthesizer;
4. Connect speakers to the synthesizer;
5. Connect a serial cable to the synthesizer;
6. Start a serial terminal at 57600-N-8-1;
7. Program the FPGA using the PlanAhead project "rtl/mdsynth.ppr" (Spartan-3A) "rtl/mdsynth-s3estarter" (Spartan-3E);
8. On the serial terminal or on the synthesizer itself at the SYS09BUG prompt, type "l";
9. Upload the text file "src/synth/synth.s19" using your serial terminal (Note: Forget the two "WHAT?" messages after the upload);
10. At the SYS09BUG prompt, type "CTRL-P" to set the program counter;
11. Type "1000" (this is the starting address of the synthesizer project);
12. Type "g". You will see the user interface on the monitor of your synthesizer system;
13. Follow the on-screen instructions in order to play and change parameters;
14. Optional: Quit your serial terminal and start MidiSerial in order to play with your MIDI keyboard (see 2.3);
14. Press the South button in order to go back to the SYS09BUG prompt.

2.2. Audio Outputs
==================

2.2.1. Spartan-3A
=================

There are two audio outputs:
	- Standard audio output connector of the board;
	- Auxiliary audio ouput on J18, pins AA21 and AB21 being left and right channels respectively.

2.2.2. Spartan-3E
=================
	- Main audio output on J1, pins B4 and A4 being left and right audio channels respectively;
	- Auxiliary audio output on J2, pins A6 and B6 being left and right audio channels respectively.
	
You will need a RC filter at the output in order to see the waveform with an oscilloscope. Refer to the Xilinx application note 154 for more details about the DAC.

2.3. MIDI Input from Serial Port
================================

The synthesizer is able to receive the MIDI note on/note off events from the serial port at 57600-N-8-1. A tool named MidiSerial allowing such serial interface is available here: http://blipbox.org/blog/projects/midiserial/.

============================================================
3. Test benches
============================================================

The test benches include a subset of the source files in order to quicky test some components of the synthesizer.

3.1. Channel Test Bench
=======================

Description
-----------

The pitch requested from the channel is increasing over time from 0 to 127.

See section 2.2 for details about the audio outputs.

This test has a single voice with a single channel per voice. Each channel has a pitch control and a waveform selection. The same channel is connected to both the left and right audio outputs.

Waveform selection
------------------

SW0	SW1	SW2	Waveform
---	---	---	--------
0	0	0	None
0	1	0	Square (message only)
1	0	0	Sawtooth (message only)
1	1	0	Sine (message only)
0	0	1	Frequency Modulation (implemented as phase modulation for simplicity)

Pitch
-----

The pitch is MIDI compatible. The value is between 0 and 127, 69 being A4 at 440Hz.

============================================================
4. Implementation Notes
============================================================

4.1 NCO
=======

I am using an numerically-controlled oscillator (NCO) in order to produce a phase value used as an index of the sine table.

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


4.2 Phase Modulation
====================

The formula is the following: y(t) = modulated_gain * sin(m(t) + 2*pi*freq_carrier*t)
where m(t) = message_gain * sin(2*pi*freq_message*t)

4.3 Monophonic Pitch Stack
==========================

The synthesizer has currently only a single voice, but we need to keep track of all notes being held in order to always play the latest held note, if any. In order to achieve this, a stack is used, the tip of the stack being the currently played pitch. When a note is release, the corresponding entry is removed from the stack. The first element of the stack is has a pitch of zero (quiet).

4.4 Gain Calculation without Multiplications
============================================

It is possible to use only additions for the gain calculation using this method:

We know that:
	log(e^a * e^b) = log(e^(a+b)) = a + b
If gain = e^a and sine = e^b, we have:
	log(gain * sine) = log(gain) + log(sine)
	gain*sine = e^(log(gain) + log(sine))

We want the gain*sine result using lookup tables as shown here:
	result(gain, phi) = exptable(loggain(gain) + logsine(phi));
	
The following will produce the tables for a 8-bit quarter cycle (copy and paste in GNU Octave):
	f1 = 64;
	logsine_size = 64;
	loggain_size = 64;
	exptable_size = (f1/32)*(256 + 256);
	wave_max = 127;

	logsine = abs(round(f1 * log(sin(linspace(0, pi/2, logsine_size))))); logsine(1) = 256 * (f1/32) - 1;
	loggain = abs(round(f1 * log(linspace(0, 1, loggain_size)))); loggain(1) = 256 * (f1/32) - 1;
	exptable = round(wave_max * exp(linspace(0, exptable_size - 1, exptable_size)/ -f1));

# In order to compare the 8-bit result (blue) with a full-resolution sine (red):
	
	gain = 1.0
	hold off
	plot(exptable(logsine + loggain(loggain_size * gain) + 1), "b");
	hold on
	plot(gain * wave_max * sin(linspace(0, pi/2, logsine_size)), "r");
	hold off

# output to CSV files

	csvwrite("logsine.csv", logsine);
	csvwrite("loggain.csv", loggain);
	csvwrite("exptable.csv", exptable);

============================================================
5. Known Issues
============================================================

- On the Spartan-3A Starter Kit with 3-bit for RGB components, the video brightness is lower than on the Spartan-3E because I drive only the MSB bit.
