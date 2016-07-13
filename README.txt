MDSynth
FPGA-based synthesizer system for the Xilinx Spartan-3A and Spartan-3E Starter Kits

Author: Daniel Cliche (dcliche@meldora.com)
Copyright (c) 2011-2016, Meldora Inc. All rights reserved.

License: BSD for the sound synthesizer chip *only*, GPL for the complete 6809-based synthesizer system.
Languages: VHDL, 6809 assembly, C

============================================================
1. Overview
============================================================

The sound chip includes a delta-sigma DAC and two numerically-controlled oscillators. It integrates a 6809-based system with keyboard support and video display output. The conversion from MIDI pitch to phase delta per clock with octave value is done by the 6809-based system for flexibility.
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

The S19 file is to be uploaded to the board using a serial terminal at 57600-N-8-1 (see the installation section). The ROM contains the SYS09BUG utility available under "src/sys09bug" which perform the download of the file over the serial port.

The chain is the following:
	"C" -- [ C compiler ] --> "ASM" -- [ 6809 assembler ] --> "S19" -- [ Serial upload with SYS09BUG ROM ] --> FPGA Block RAM Memory

The C compiler based on the public domain Micro-C is tiny (around 3K lines of code in a single file) and is quite impressive for its size. Obviously, no optimization is done and everything is done in a single pass, so there is no separation between data and code. It also only implement a subset of the C language.

If you plan to use this tiny C compiler, please read the README file located the project for known limitations and issues I encountered.
	
2.2. Sound Chip Addresses
=========================
	
Two sound chips are available at the following base addresses:
- 0xE080	Left Audio
- 0xE090	Right Audio

Base + 0: 
	 bits 2 downto 0: Waveform (0x00: DAC direct, 0x01: Square, 0x02: Sawtooth, 0x03: Sine, 0x04: Phase Modulation)
	 bit 7: Note ON/OFF (0: OFF, 1: ON)
Base + 1: Message Octave (4-bit) + Phase Delta MSB (4-bit)
Base + 2: Phase Delta LSB (8-bit)
Base + 3: Carrier Octave (4-bit)
Base + 4: Message Gain (6-bit, 63=1.0, sine and phase modulation only)
Base + 5: Modulated Gain (6-bit, 63=1.0, phase modulation only)
Base + 6: DAC direct value (8-bit, DAC direct only)

Refer to section 4.1 for more details about the 12-bit phase delta value to provide to the NCO along with the 4-bit octave value.

2.3. Installation
=================

1. Compile the tools located in "src/tools" using the Visual C++ 2015 project.

2. Open a command prompt, go the the "src/synth" directory and execute "make". This command will produce the "synth.s19" file to upload.
3. Connect a VGA monitor to the synthesizer (your Xilinx Spartan 3A-Starter Kit board);
4. Connect a PS/2 keyboard to the synthesizer;
5. Connect speakers to the synthesizer;
6. Connect a serial cable to the synthesizer;
7. Program the FPGA by creating a projet with the files under the "rtl" folder;
8. Start a serial terminal at 57600-N-8-2;
9. On the serial terminal or on the synthesizer itself at the SYS09BUG prompt, type "l";
10. Upload the text file "src/synth/synth.s19" using your serial terminal;
11. At the SYS09BUG prompt, type "CTRL-P" to set the program counter;
12. Type "1000" (this is the starting address of the synthesizer project);
13. Type "g". You will see the user interface on the monitor of your synthesizer system;
14. Follow the on-screen instructions in order to play and change parameters;
15. Press the South button in order to go back to the SYS09BUG prompt.


2.2. Audio Outputs and MIDI I/O
===============================

2.2.1. Spartan-3A
=================
	- Standard audio output connector of the board;
	- Auxiliary audio ouput on J18, pins AA21 and AB21 being left and right channels respectively.
	- MIDI Input on J18, pin AA19
	- MIDI Output on J18, pin AB19

2.2.2. Spartan-3E
=================
	- Main audio output on J1, pins B4 and A4 being left and right audio channels respectively;
	- Auxiliary audio output on J2, pins A6 and B6 being left and right audio channels respectively.
	- MIDI Input on J2, pin E7
	- MIDI Output on J2, pin F7
	
You will need a RC filter at the output in order to see the waveform with an oscilloscope. Refer to the Xilinx application note 154 for more details about the DAC.

2.3. MIDI Interface
===================

The synthesizer is able to receive the MIDI note on/note off events from a MIDI device. Here's the schematic of a MIDI IN interface:

                                        +--6N138--+
MIDI IN [4] ----- 220 -----------------[2]       [8]--------------- VCC 3.3V
                          |             |         |           |
                        1N4148          |         |          220
                          |             |         |           |
MIDI IN [5] ---------------------------[3]       [6]--------------- FPGA MIDI INPUT
                                        |         |
                                        |        [5]--- GND
                                        +---------+

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
0	0	1	Phase Modulation

In phase modulation mode, press the north button for Note ON (Note: button not debounced).

Pitch
-----

The pitch is MIDI compatible. The value is between 0 and 127, 69 being A4 at 440Hz.

============================================================
4. Implementation Notes
============================================================

4.1 NCO
=======

I am using an numerically-controlled oscillator (NCO) in order to produce a phase value used as an index of the sine table.

pitch 69 (A4) will require the following: octave=6, note=9 
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


4.2 Phase Modulation
====================

The formula is the following: y(t) = modulated_gain * sin(pi*m(t) + 2*pi*octave_carrier*freq*t)
where m(t) = message_gain * sin(2*pi*octave_message*freq*t)

4.3 Monophonic Pitch Stack
==========================

The synthesizer has currently only a single voice, but we need to keep track of all notes being held in order to always play the latest held note, if any. In order to achieve this, a stack is used, the tip of the stack being the currently played pitch. When a note is release, the corresponding entry is removed from the stack. The first element of the stack has a pitch of zero (quiet).

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
	logsine_size = 65;
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
5. Limitations and Known Issues
============================================================

- Envelope rate are fixed for now;
- Gain for modulated signal is not used.
