-- MDSynth Sound Chip
--
-- Copyright (c) 2012, Meldora Inc.
-- All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
-- following conditions are met:
--
--    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
--      following disclaimer.
--    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
--      following disclaimer in the documentation and/or other materials provided with the distribution.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
-- INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
-- DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
-- SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
-- SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
-- WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
-- USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

--
-- Numerically-controlled oscillator
--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

-- freq = (50E6 * (phase_delta * 2^octave)) / 2^32
entity nco is
    port ( clk:         in std_logic;
           reset_phase: in std_logic;
           ena:         in std_logic;
           phase_delta: in unsigned(11 downto 0);
           octave:      in unsigned(3 downto 0);
           phase:       out unsigned(7 downto 0));
end nco;

architecture nco_arch of nco is

signal phase_accumulator: unsigned(31 downto 0) := to_unsigned(0, 32);
signal toggle: std_logic := '0' ;
signal phase_delta_32: unsigned(31 downto 0) := to_unsigned(0, 32);

begin

    process (clk)
	begin
		if (rising_edge(clk)) then
            phase_delta_32(11 downto 0) <= phase_delta;
            if (reset_phase = '1') then
                phase_accumulator <= to_unsigned(0, 32);
            elsif (ena = '1') then
		        phase_accumulator <= phase_accumulator + (phase_delta_32 sll (to_integer(octave)));
		    end if;
        	phase <= phase_accumulator(31 downto 24);
		end if;
	end process;
	
end nco_arch;
