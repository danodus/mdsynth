MD-Synthesizer
FPGA-based synthesizer for the Xilinx Spartan-3A Starter Kit

Author: Daniel Cliche (dcliche@meldora.com)
Copyright (c) 2011, Meldora Inc. All rights reserved.

License: New BSD
Language: VHDL

Description
-----------

In its current state, the project is a simple wave generator using a sigma-delta DAC. The ouput pin of the DAC is either AA21 or AB21. You will need a RC filter at the output. Refer to the Xilinx application note 154 for more details about the DAC.

In this version, we have a single voice with a single channel per voice. Each voice has a pitch control and a waveform selection. The same channel is connected to both the left and right audio outputs.

In this example, the pitch requested from the channel is increasing over time from 0 to 127.

The projet has been converted to Xilinx PlanAhead 13.1.

Waveform selection
------------------

SW0	SW1	Waveform
--	--	--------
0	0	None
0	1	Square
1	0	Sawtooth
1	1	Sine

Pitch
-----

The pitch is MIDI compatible. The value is between 0 and 127, 69 being A4 at 440Hz.

Limitations
-----------

Only the square wave will be at correct pitch in this version. The other waveforms will require a higher frequency from the pitch to clock converter.

