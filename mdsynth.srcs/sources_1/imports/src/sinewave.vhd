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

entity sinewave is
	port (  phase:    in  unsigned(7 downto 0);
			data_out: out integer range -128 to 127);
end sinewave;

architecture Behavioral of sinewave is
signal i : integer range 0 to 30:=0;
type memory_type is array (0 to 255) of integer range -128 to 127;
signal sine : memory_type :=(0,3,6,9,12,15,18,21,24,28,31,34,37,40,43,46,48,51,54,57,60,63,65,68,71,73,76,78,81,83,85,88,90,92,94,96,98,100,102,104,106,108,109,111,112,114,115,117,118,119,120,121,122,123,124,124,125,126,126,127,127,127,127,127,127,127,127,127,127,127,126,126,125,124,124,123,122,121,120,119,118,117,115,114,112,111,109,108,106,104,102,100,98,96,94,92,90,88,85,83,81,78,76,73,71,68,65,63,60,57,54,51,48,46,43,40,37,34,31,28,24,21,18,15,12,9,6,3,0,-4,-7,-10,-13,-16,-19,-22,-25,-29,-32,-35,-38,-41,-44,-47,-49,-52,-55,-58,-61,-64,-66,-69,-72,-74,-77,-79,-82,-84,-86,-89,-91,-93,-95,-97,-99,-101,-103,-105,-107,-109,-110,-112,-113,-115,-116,-118,-119,-120,-121,-122,-123,-124,-125,-125,-126,-127,-127,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-128,-127,-127,-126,-125,-125,-124,-123,-122,-121,-120,-119,-118,-116,-115,-113,-112,-110,-109,-107,-105,-103,-101,-99,-97,-95,-93,-91,-89,-86,-84,-82,-79,-77,-74,-72,-69,-66,-64,-61,-58,-55,-52,-49,-47,-44,-41,-38,-35,-32,-29,-25,-22,-19,-16,-13,-10,-7,-4);

begin
	data_out <= sine(to_integer(phase));
end Behavioral;