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

entity channel is
    port ( clk:      in  std_logic;
           reset:    in std_logic;
           waveform: in unsigned(1 downto 0);    -- 0: None, 1: Square, 2: Sawtooth, 3: Sine
           pitch:    in unsigned(6 downto 0);      -- 60 = C4
           output:   out  std_logic);
end channel;

architecture channel_arch of channel is

component dac is
	port (	clk:		in std_logic;
			dac_in:	    in std_logic_vector(7 downto 0);
			reset:	    in std_logic;
			dac_out:	out std_logic);
end component;

component sinewave is
	port (    clk:			in  std_logic;
			  data_out: 	out integer range -128 to 127);
end component;

component pitch_clk_div is
    port ( clk: std_logic;
           pitch : in unsigned(6 downto 0);      -- 60 = C4
           output : out  std_logic);
end component;

signal counter : unsigned(15 downto 0) := (others => '0');

signal dac_in : std_logic_vector(7 downto 0);
signal dac_reset : std_logic;
signal sine : integer range -128 to 127 := 0;
signal clk_div_out : std_logic;


begin

    pitch_clk_div0 : pitch_clk_div port map (clk => clk, pitch => pitch, output => clk_div_out);
	sinewave0 : sinewave port map (clk => clk_div_out, data_out => sine);
	dac0 : dac port map (clk => clk, dac_in => dac_in, reset => reset, dac_out => output);

	process (clk_div_out)
	begin
	    if (clk_div_out'event and clk_div_out = '1') then
			counter <= counter + 1;
            case waveform is
                when "01" =>
                    -- Square wave
                    if (std_logic(counter(0))= '1') then
                        dac_in(7 downto 0) <= (others => '1');
                    else
                        dac_in(7 downto 0) <= (others => '0');
                    end if;
                when "10" =>
                    -- Sawtooth wave
                    dac_in <= std_logic_vector(counter(7 downto 0));
                when "11" =>
                    -- Sine wave
                    dac_in <= conv_std_logic_vector(128 + sine, 8);
                when others =>
                    dac_in(7 downto 0) <= (others => '0');
            end case;
		end if;
		
	end process;
	
end channel_arch;

