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
use IEEE.STD_LOGIC_SIGNED.ALL;

entity sinewave is
	port (clk 		: in  std_logic;
			data_out : out integer range -128 to 127);
end sinewave;

architecture Behavioral of sinewave is
signal i : integer range 0 to 30:=0;
type memory_type is array (0 to 29) of integer range -128 to 127;
signal sine : memory_type :=(0,16,31,45,58,67,74,77,77,74,67,58,45,31,16,0,-16,-31,-45,-58,-67,-74,-77,-77,-74,-67,-58,-45,-31,-16);

begin

	process(clk)
		begin
		if(rising_edge(clk)) then    
			data_out <= sine(i);
			i <= i+ 1;
			if(i = 29) then
				i <= 0;
			end if;
		end if;
	end process;

end Behavioral;