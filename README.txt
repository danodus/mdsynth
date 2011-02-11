MD-Synthesizer
FPGA-based synthesizer for the Xilinx Spartan-3A Starter Kit

Author: Daniel Cliche (dcliche@meldora.com)
Copyright (c) 2011, Meldora Inc. All rights reserved.

License: New BSD
Language: VHDL

Description
-----------

In its current state, the project is a simple wave generator using a sigma-delta DAC. The ouput pin of the DAC is either AA21 or AB21. You will need a RC filter at the output. Refer to the Xilinx application note 154 for more details about the DAC.

Waveform selection
------------------

SW0	SW1	Waveform
--	--	--------
0	0	None
0	1	Square
1	0	Sawtooth
1	1	Sine

Note: In this preliminary version, the frequency is fixed.
