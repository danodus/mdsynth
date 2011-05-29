-- MD-Synthesizer
--
-- Author: Daniel Cliche (dcliche@meldora.com)
-- Copyright (c) 2011, Meldora Inc. All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions are met:
--    * Redistributions of source code must retain the above copyright
--      notice, this list of conditions and the following disclaimer.
--    * Redistributions in binary form must reproduce the above copyright
--      notice, this list of conditions and the following disclaimer in the
--      documentation and/or other materials provided with the distribution.
--    * Neither the name of Meldora Inc. nor the
--      names of its contributors may be used to endorse or promote products
--      derived from this software without specific prior written permission.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
-- ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
-- WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
-- DISCLAIMED. IN NO EVENT SHALL MELDORA INC. BE LIABLE FOR ANY
-- DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
-- (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
-- LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
-- ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
-- (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
-- SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

--
-- Numerically-controlled oscillator with MIDI-compatible pitch input
--



library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity nco is
    port ( clk:     in std_logic;
           pitch:   in unsigned(6 downto 0);      -- 60 = C4
           phase:   out unsigned(7 downto 0));
end nco;

architecture nco_arch of nco is

component div_by_12 is
    port ( numerator : in  unsigned(6 downto 0);
           quotient : out  unsigned(3 downto 0);
           remain : out  unsigned(3 downto 0));
end component;

signal octave: unsigned(3 downto 0);
signal note: unsigned(3 downto 0);
	
signal phase_accumulator: unsigned(31 downto 0) := to_unsigned(0, 32);
signal phase_delta: unsigned(31 downto 0) := to_unsigned(0, 32);

signal toggle: std_logic := '0';

begin
	div0: div_by_12 port map (numerator => pitch, quotient => octave, remain => note);

    -- pitch 69 (A4) will give the following: octave=5, note=9 
    -- The desired frequencies for octave 5 are the following:
    --    note     freq (Hz)
    --    0        261.63    (C4)
    --    1        277.18
    --    2        293.66
    --    3        311.13
    --    4        329.63
    --    5        349.23
    --    6        369.99
    --    7        392.00
    --    8        415.30
    --    9        440.00    (A4)
    --    10       466.16
    --    11       493.88

	process (note)
	begin
		case note is
			when "0000" =>	phase_delta <= to_unsigned(22474*32, 32);		-- C10
			when "0001" =>	phase_delta <= to_unsigned(23810*32, 32);		-- C10#
			when "0010" =>	phase_delta <= to_unsigned(25225*32, 32);		-- D10
			when "0011" =>	phase_delta <= to_unsigned(26726*32, 32);		-- D10#
			when "0100" =>	phase_delta <= to_unsigned(28315*32, 32);		-- E10
			when "0101" =>	phase_delta <= to_unsigned(29999*32, 32);		-- F10
			when "0110" =>	phase_delta <= to_unsigned(31782*32, 32);		-- F10#
			when "0111" =>	phase_delta <= to_unsigned(33673*32, 32);		-- G10
			when "1000" =>	phase_delta <= to_unsigned(35674*32, 32);		-- G10#
			when "1001" =>	phase_delta <= to_unsigned(37796*32, 32);		-- A10
			when "1010" =>	phase_delta <= to_unsigned(40043*32, 32);		-- A10#
			when "1011" =>	phase_delta <= to_unsigned(42424*32, 32);		-- B10
			when others =>  phase_delta <= to_unsigned(0, 32);			    -- Should never happen
		end case;
	end process;

    process (clk)
	begin
		if (rising_edge(clk)) then
		    phase_accumulator <= phase_accumulator + (phase_delta srl (10 - to_integer(octave)));
		end if;
	end process;
	
	phase <= phase_accumulator(31 downto 24);
	
end nco_arch;