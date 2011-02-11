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


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.NUMERIC_STD.ALL;

entity mdsynth is
   port ( clk      : in    std_logic; 
 			 btn_south: in		std_logic;
			 switch	 : in		std_logic_vector(3 downto 0);
			 aud_l    :	out std_logic;
			 aud_r    :	out std_logic);
 
end entity mdsynth;

architecture mdsynth_arch of mdsynth is

component dac is
	port (	clk:		in std_logic;
				dac_in:	in std_logic_vector(7 downto 0);
				reset:	in std_logic;
				dac_out:	out std_logic);
end component;

component sinewave is
	port (clk :			in  std_logic;
			data_out : 	out integer range -128 to 127
			);
end component;


signal counter : unsigned(15 downto 0) := (others => '0');

signal dac_in : std_logic_vector(7 downto 0);
signal dac_reset : std_logic;
signal dac_out : std_logic;
signal sine_clk : std_logic;
signal sine : integer range -128 to 127;

begin

	dac1 : dac port map (clk => clk, dac_in => dac_in, reset => dac_reset, dac_out => dac_out);
	sinewave1 : sinewave port map (clk => sine_clk, data_out => sine);

	process (clk)
	begin
		if (clk'event and clk = '1') then
			counter <= counter + 1;
		end if;
	end process;
	
	process (switch)
	begin
		case switch(1 downto 0) is
			when B"01" =>
				-- Square wave
				dac_in(7 downto 0) <= (others => std_logic(counter(15)));
			when B"10" =>
				-- Sawtooth wave
				dac_in <= std_logic_vector(counter(15 downto 8));
			when B"11" =>
				-- Sine wave
				dac_in <= conv_std_logic_vector(128 + sine, 8);
			when others =>
				dac_in(7 downto 0) <= (others => '0');
		end case;
	end process;
	
	sine_clk <= std_logic(counter(10));
	dac_reset <= btn_south;
	aud_l <= dac_out;
	aud_r <= dac_out;
		
end architecture mdsynth_arch;
