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
-- Based on the Music Box example at www.fpga4fun.com
--



library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;

entity pitch_clk_div is
    port ( clk: std_logic;
           pitch : in unsigned(6 downto 0);      -- 60 = C4
           output : out  std_logic);
end pitch_clk_div;

architecture pitch_clk_div_arch of pitch_clk_div is

component div_by_12 is
    port ( numerator : in  unsigned(6 downto 0);
           quotient : out  unsigned(3 downto 0);
           remain : out  unsigned(3 downto 0));
end component;

signal octave: unsigned(3 downto 0);
signal note: unsigned(3 downto 0);
signal note_divider: unsigned(10 downto 0);
signal octave_divider: unsigned(9 downto 0);
	
signal counter_note: unsigned(10 downto 0) := conv_unsigned(0, 11);
signal counter_octave: unsigned(9 downto 0) := conv_unsigned(0, 10);

signal toggle: std_logic := '0';

begin
	div0: div_by_12 port map (numerator => pitch, quotient => octave, remain => note);

    -- pitch 69 (A4) will give the following: octave=5, note=9 
    -- Resulting frequency at the output = 50MHz / (4 * note_divider * octave_divider)
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
    -- We fix the octave_divider to 32, so note_divider = 50MHz / (4 * freq * 32)

	process (note)
	begin
		case note is
			when "0000" =>	note_divider <= conv_unsigned(1493, 11);		-- C
			when "0001" =>	note_divider <= conv_unsigned(1409, 11);		-- C#
			when "0010" =>	note_divider <= conv_unsigned(1330, 11);		-- D
			when "0011" =>	note_divider <= conv_unsigned(1256, 11);		-- D#
			when "0100" =>	note_divider <= conv_unsigned(1185, 11);		-- E
			when "0101" =>	note_divider <= conv_unsigned(1119, 11);		-- F
			when "0110" =>	note_divider <= conv_unsigned(1056, 11);		-- F#
			when "0111" =>	note_divider <= conv_unsigned(996, 11);		    -- G
			when "1000" =>	note_divider <= conv_unsigned(941, 11);		    -- G#
			when "1001" =>	note_divider <= conv_unsigned(888, 11);		    -- A
			when "1010" =>	note_divider <= conv_unsigned(838, 11);		    -- A#
			when "1011" =>	note_divider <= conv_unsigned(791, 11);		    -- B
			when others =>  note_divider <= conv_unsigned(0, 11);			    -- Should never happen
		end case;
	end process;
	
	process (octave)
	begin
		case octave is
			when "0000" =>	octave_divider <= conv_unsigned(1024 - 1, 10);
			when "0001" =>	octave_divider <= conv_unsigned(512 - 1, 10);
			when "0010" =>	octave_divider <= conv_unsigned(256 - 1, 10);
			when "0011" =>	octave_divider <= conv_unsigned(128 - 1, 10);
			when "0100" =>	octave_divider <= conv_unsigned(64 - 1, 10);
			when "0101" =>   octave_divider <= conv_unsigned(32 - 1, 10);
			when "0110" =>   octave_divider <= conv_unsigned(16 - 1, 10);
			when "0111" =>   octave_divider <= conv_unsigned(8 - 1, 10);
			when "1000" =>   octave_divider <= conv_unsigned(4 - 1, 10);
			when "1001" =>   octave_divider <= conv_unsigned(2 - 1, 10);
			when "1010" =>   octave_divider <= conv_unsigned(1 - 1, 10);
			when others =>  octave_divider <= conv_unsigned(0, 10);			-- Should never happen
		end case;
	end process;
	
	process (clk)
	begin
		if (clk'event and clk = '1') then
			if (counter_note = 0) then
				counter_note <= note_divider;
			else
				counter_note <= counter_note - 1;
			end if;
		end if;
	end process;
	
	process (clk)
	begin
		if (clk'event and clk = '1') then
			if (counter_note = 0) then
				if (counter_octave = 0) then
				    counter_octave <= octave_divider;
				else
					counter_octave <= counter_octave - 1;
				end if;
			end if;
		end if;
	end process;
	
	process (clk)
	begin
	    if (clk'event and clk = '1') then
	        if (counter_note = 0 and counter_octave = 0) then
	            toggle <= not toggle;
	        end if;
	    end if;
	end process;
	
	output <= toggle;

end pitch_clk_div_arch;