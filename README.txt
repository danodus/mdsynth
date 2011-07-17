MDSynth
FPGA-based synthesizer system for the Xilinx Spartan-3A Starter Kit

Author: Daniel Cliche (dcliche@meldora.com)
Copyright (c) 2011, Meldora Inc. All rights reserved.

License: New BSD for the sound synthesizer chip *only*, GPL for the complete 6809-based synthesizer system.
Languages: VHDL, 6809 assembly, C

============================================================
1. Overview
============================================================

In its current state, the sound chip is a simple wave generator using a sigma-delta DAC. It integrates a 6809-based system with keyboard support and video display output. The conversion to MIDI pitch is currently done by the sound chip itself, but will eventually be done by the 6809-based system for flexibility.

============================================================
2. Complete synthesizer as a 6809-based system
============================================================

Adaptation of the excellent System09 project by John E. Kent (dilbert57@opencores.org)
http://members.optushome.com.au/jekent/system09/

A public domain C compiler has been added to the tools. All the source files are provided, including the toolset which include the following:
- A 6809 assembler (tools/as09);
- A C compiler (tools/mc09);
- S19 to VHDL (tools/s19tovhd) for producing the ROM file.

The toolset executables are included for Windows, but you may want to compile them for Linux.

The S19 file is to be uploaded to the board using a serial terminal at 57600-N-8-1 (see the installation section). The ROM contains the SYS09BUG utility available under "src/sys09bug" which perform the download of the file over the serial port.

The chain is the following:
	"C" -- [ C compiler ] --> "ASM" -- [ 6809 assembler ] --> "S19" -- [ Serial upload with SYS09BUG ROM ] --> FPGA Block RAM Memory
	
Two sound chips are available at the following base addresses:
- 0xE080	Left Audio
- 0xE090	Right Audio

Base + 0: Waveform (0x00: None, 0x01: Square, 0x02: Sawtooth, 0x03: Sine, 0x04: FM)
Base + 1: Message Pitch (currently MIDI pitch, but will eventually change)
Base + 2: Carrier Pitch (currently MIDI pitch, but will eventually change)


2.1. Installation
=================

1. Connect a VGA monitor to the synthesizer (your Xilinx Spartan 3A-Starter Kit board);
2. Connect a PS/2 keyboard to the synthesizer;
3. Connect speakers to the synthesizer;
4. Connect a serial cable to the synthesizer;
5. Start a serial terminal at 57600-N-8-1;
6. Program the FPGA using the PlanAhead project "rtl/mdsynth.ppr";
7. On the serial terminal or on the synthesizer itself at the SYS09BUG prompt, type "l";
8. Upload the text file "src/synth/synth.s19" using your serial terminal (Note: Forget the two "WHAT?" messages after the upload);
9. At the SYS09BUG prompt, type "CTRL-P" to set the program counter;
10. Type "1000" (this is the starting address of the synthesizer project);
11. Type "g". You will see "MD-Synthesizer" at the top-left on the monitor of your synthesizer system;
12. On the synthesizer, press "2" to switch to carrier pitch;
13. On the synthesizer, press any other key to play (the played pitch is the ASCII value of the key for now);
14. Press the South button to go back to the SYS09BUG prompt.

============================================================
3. Test benches
============================================================

The test benches includes a subset of the source files in order to quicky test some components of the synthesizer.

3.1. Channel Test Bench
=======================

Description
-----------

The pitch requested from the channel is increasing over time from 0 to 127.

There are two audio outputs:
	- Standard audio output of the board;
	- Auxiliary audio ouput on J18, pins AA21 and AB21 being left and right channels respectively.
	
You will need a RC filter at the output in order to see the waveform with an oscilloscope. Refer to the Xilinx application note 154 for more details about the DAC.

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

4.2 Phase Modulation
====================

The formula is the following: y(t) = sin(m(t) + 2*pi*freq_carrier*t)
where m(t) = sin(2*pi*freq_message*t)
