MD-Synthesizer
FPGA-based synthesizer for the Xilinx Spartan-3A Starter Kit

Author: Daniel Cliche (dcliche@meldora.com)
Copyright (c) 2011, Meldora Inc. All rights reserved.

License: GPL for the complete system, New BSD for the sound synthesizer module only.
Languages: VHDL, 6809 assembly, C

============================================================
Overview
============================================================

The project is divided in two components:
	- Synth: The sound synthesizer module with a test bench;
	- Sys09: The complete 6809-based system with sound synthesizer module integration.

Each component has its own PlanAhead project.

============================================================
Sound synthesizer module
============================================================

License: New BSD (if used as a stand-alone component)
Languages: VHDL
Path: synth

Description
-----------

In its current state, the project is a simple wave generator using a sigma-delta DAC. There are two audio outputs:
	- Standard audio output of the board;
	- Auxiliary audio ouput on J18, pins AA21 and AB21 being left and right channels respectively.
	
You will need a RC filter at the output in order to see the waveform with an oscilloscope. Refer to the Xilinx application note 154 for more details about the DAC.

In this version, we have a single voice with a single channel per voice. Each voice has a pitch control and a waveform selection. The same channel is connected to both the left and right audio outputs.

The pitch requested from the channel is increasing over time from 0 to 127.

The projet has been converted to Xilinx PlanAhead 13.1.

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

Phase Modulation
----------------

The formula is the following: y(t) = sin(m(t) + 2*pi*freq_carrier*t)
where m(t) = sin(2*pi*freq_message*t)

Implementation Notes
--------------------

I am using an numerically-controlled oscillator (NCO) in order to produce a phase value used as an index of the sine table.

============================================================
6809-based synthesizer system
============================================================

Adaptation of the excellent System09 project by John E. Kent (dilbert57@opencores.org)
http://members.optushome.com.au/jekent/system09/

License: GPL
Languages: VHDL, 6809 assembly, C
Path:	sys09
