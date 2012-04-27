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
-- MIDI-compatible pitch to NCO-compatible frequency with phase delta and octave
--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity pitch_to_freq is
    port ( pitch:         in unsigned(6 downto 0);      -- 60 = C4
           phase_delta:   out unsigned(11 downto 0);
           octave:        out unsigned(3 downto 0));
end pitch_to_freq;

architecture pitch_to_freq_arch of pitch_to_freq is

component div_by_12 is
    port ( numerator : in  unsigned(6 downto 0);
           quotient : out  unsigned(3 downto 0);
           remain : out  unsigned(3 downto 0));
end component;

signal note: unsigned(3 downto 0);	
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
    
    -- The frequency given to NCO is the following:
    -- freq = (50E6 * (phase_delta * 2^octave)) / 2^32
    -- phase_delta = freq * 2^32 / (50E6 * 2^octave)

	process (note)
	begin
		case note is
			when "0000" =>	phase_delta <= to_unsigned(702, 12);		-- C4
			when "0001" =>	phase_delta <= to_unsigned(744, 12);		-- C4#
			when "0010" =>	phase_delta <= to_unsigned(788, 12);		-- D4
			when "0011" =>	phase_delta <= to_unsigned(835, 12);		-- D4#
			when "0100" =>	phase_delta <= to_unsigned(885, 12);		-- E4
			when "0101" =>	phase_delta <= to_unsigned(937, 12);		-- F4
			when "0110" =>	phase_delta <= to_unsigned(993, 12);		-- F4#
			when "0111" =>	phase_delta <= to_unsigned(1052, 12);		-- G4
			when "1000" =>	phase_delta <= to_unsigned(1115, 12);		-- G4#
			when "1001" =>	phase_delta <= to_unsigned(1181, 12);		-- A4
			when "1010" =>	phase_delta <= to_unsigned(1251, 12);		-- A4#
			when "1011" =>	phase_delta <= to_unsigned(1326, 12);		-- B4
			when others =>  phase_delta <= to_unsigned(0, 12);		    -- Should never happen
		end case;
	end process;
	
end pitch_to_freq_arch;
