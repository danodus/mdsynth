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

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity sinewave is
	port (  clk:      in std_logic;
	        gain:     in unsigned(5 downto 0);
	        phase:    in unsigned(7 downto 0);
			data_out: out integer range -128 to 127);
end sinewave;

architecture Behavioral of sinewave is

type log_sine_type is array (0 to 64) of integer range 0 to 511;
type log_gain_type is array (0 to 63) of integer range 0 to 511;
type exp_table_type is array (0 to 1023) of integer range -128 to 127;

signal log_sine : log_sine_type := (511,237,193,167,149,134,123,113,105,97,91,85,79,74,70,65,61,58,54,51,48,45,43,40,38,35,33,31,29,27,25,24,22,21,19,18,16,15,14,13,12,11,10,9,8,7,6,6,5,4,4,3,3,2,2,2,1,1,1,0,0,0,0,0,0);
signal log_gain : log_gain_type := (511,265,221,195,176,162,150,141,132,125,118,112,106,101,96,92,88,84,80,77,73,70,67,64,62,59,57,54,52,50,47,45,43,41,39,38,36,34,32,31,29,27,26,24,23,22,20,19,17,16,15,14,12,11,10,9,8,6,5,4,3,2,1,0);
signal exp_table : exp_table_type := (127,125,123,121,119,117,116,114,112,110,109,107,105,104,102,100,99,97,96,94,93,91,90,89,87,86,85,83,82,81,79,78,77,76,75,74,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,55,54,53,52,51,51,50,49,48,47,47,46,45,45,44,43,43,42,41,41,40,39,39,38,38,37,36,36,35,35,34,34,33,33,32,32,31,31,30,30,29,29,28,28,27,27,27,26,26,25,25,25,24,24,23,23,23,22,22,22,21,21,21,20,20,20,19,19,19,19,18,18,18,17,17,17,17,16,16,16,16,15,15,15,15,14,14,14,14,14,13,13,13,13,13,12,12,12,12,12,11,11,11,11,11,11,10,10,10,10,10,10,9,9,9,9,9,9,9,9,8,8,8,8,8,8,8,8,7,7,7,7,7,7,7,7,7,6,6,6,6,6,6,6,6,6,6,5,5,5,5,5,5,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

begin
    process(clk)
    begin
        if (rising_edge(clk)) then
            if (phase <= 64) then
                data_out <= exp_table(log_gain(to_integer(gain)) + log_sine(to_integer(phase)));
            elsif (phase > 64 and phase <= 128) then
                data_out <= exp_table(log_gain(to_integer(gain)) + log_sine(64 - (to_integer(phase) - 64))); 
            elsif (phase > 128 and phase <= 192) then
                data_out <= -exp_table(log_gain(to_integer(gain)) + log_sine(to_integer(phase) - 128));
            else -- phase > 192 and phase < 256
                data_out <= -exp_table(log_gain(to_integer(gain)) + log_sine(64 - (to_integer(phase) - 192)));
            end if;
        end if;
	end process;
end Behavioral;
