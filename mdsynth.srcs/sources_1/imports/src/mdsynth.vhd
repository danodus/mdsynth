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
use IEEE.NUMERIC_STD.ALL;

entity mdsynth is
   port ( clk:         in std_logic; 
 		  btn_south:   in std_logic;
          switch:      in std_logic_vector(3 downto 0);
          aud_l:       out std_logic;
          aud_r:       out std_logic;
          aux_aud_l:   out std_logic;
          aux_aud_r:   out std_logic);
 
end entity mdsynth;

architecture mdsynth_arch of mdsynth is

component channel is
    port ( clk:      in std_logic;
           reset:    in std_logic;
           waveform: in std_logic_vector(1 downto 0);    -- 0: None, 1: Square, 2: Sawtooth, 3: Sine
           pitch:    in unsigned(6 downto 0);      -- 60 = C4
           output:   out  std_logic);
end component;

signal channel_out: std_logic;
signal pitch: unsigned (6 downto 0) := to_unsigned(69, 7);
signal counter: unsigned (31 downto 0) := to_unsigned(0, 32);
signal waveform: std_logic_vector(1 downto 0) := "11";

begin

    channel0: channel port map (clk => clk, reset => btn_south, waveform => waveform, pitch => pitch, output => channel_out);

    waveform <= switch(1 downto 0);

    aud_l <= channel_out;
	aud_r <= channel_out;
	aux_aud_l <= channel_out;
	aux_aud_r <= channel_out;
	
	process (clk)
	begin
	    if (rising_edge(clk)) then
	        if (counter = 0) then
	            counter <= to_unsigned(5000000, 32);
	            pitch <= pitch + 1;
	        else
	            counter <= counter - 1;
	        end if;
	    end if;
	end process;
	
end architecture mdsynth_arch;
