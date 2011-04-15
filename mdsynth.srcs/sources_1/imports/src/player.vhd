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

entity player is
    Port ( clk : in  std_logic;
           output : out  std_logic);
end player;

architecture Behavioral of player is

component div_by_12 is
    Port ( numerator : in  unsigned(5 downto 0);
           quotient : out  unsigned(2 downto 0);
           remain : out  unsigned(3 downto 0));
end component;

signal tone: unsigned(27 downto 0);
signal full_note: unsigned(5 downto 0);
signal octave: unsigned(2 downto 0);
signal note: unsigned(3 downto 0);
signal clk_divider: unsigned(8 downto 0);
	
signal counter_note: unsigned(8 downto 0);
signal counter_octave: unsigned(7 downto 0);

signal toggle: std_logic := '0';

begin

	div1: div_by_12 port map (numerator => full_note, quotient => octave, remain => note);

	process (note)
	begin
		case note is
			when "0000" =>	clk_divider <= conv_unsigned(512 - 1, 9);		-- A
			when "0001" =>	clk_divider <= conv_unsigned(483 - 1, 9);		-- A#/Bb
			when "0010" =>	clk_divider <= conv_unsigned(456 - 1, 9);		-- B
			when "0011" =>	clk_divider <= conv_unsigned(431 - 1, 9);		-- C
			when "0100" =>	clk_divider <= conv_unsigned(406 - 1, 9);		-- C#/Db
			when "0101" =>	clk_divider <= conv_unsigned(384 - 1, 9);		-- D
			when "0110" =>	clk_divider <= conv_unsigned(362 - 1, 9);		-- D#/Eb
			when "0111" =>	clk_divider <= conv_unsigned(342 - 1, 9);		-- E
			when "1000" =>	clk_divider <= conv_unsigned(323 - 1, 9);		-- F
			when "1001" =>	clk_divider <= conv_unsigned(304 - 1, 9);		-- F#/Gb
			when "1010" =>	clk_divider <= conv_unsigned(287 - 1, 9);		-- G
			when "1011" =>	clk_divider <= conv_unsigned(271 - 1, 9);		-- G#/Ab
			when others => clk_divider <= conv_unsigned(0, 9);			-- Should never happen
		end case;
	end process;
	
	process (clk)
	begin
		if (clk'event and clk = '1') then
			if (counter_note = 0) then
				counter_note <= clk_divider;
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
					case octave is
						when "000" =>	counter_octave <= conv_unsigned(255, 8);
						when "001" =>	counter_octave <= conv_unsigned(127, 8);
						when "010" =>	counter_octave <= conv_unsigned(63, 8);
						when "011" =>	counter_octave <= conv_unsigned(31, 8);
						when "100" =>	counter_octave <= conv_unsigned(15, 8);
						when others => counter_octave <= conv_unsigned(7, 8);
					end case;
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

end Behavioral;

