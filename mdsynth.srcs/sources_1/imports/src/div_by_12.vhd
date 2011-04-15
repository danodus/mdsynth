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

entity div_by_12 is
    Port ( numerator : in  unsigned(5 downto 0);
           quotient : out  unsigned(2 downto 0);
           remain : out  unsigned(3 downto 0));
end div_by_12;

architecture Behavioral of div_by_12 is
signal r: unsigned(3 downto 0);
begin

	process (numerator(5 downto 2))
	begin
		case numerator(5 downto 2) is
			when "0000" => r <= "0000";
			when "0001" => r <= "0001";
			when "0010" => r <= "0010";
			when "0011" => r <= "0000";
			when "0100" => r <= "0001";
			when "0101" => r <= "0010";
			when "0110" => r <= "0000";
			when "0111" => r <= "0001";
			when "1000" => r <= "0010";
			when "1001" => r <= "0000";
			when "1010" => r <= "0001";
			when "1011" => r <= "0010";
			when "1100" => r <= "0000";
			when "1101" => r <= "0001";
			when "1110" => r <= "0010";
			when "1111" => r <= "0000";
			when others => r <= "0000"; -- Should not happen
		end case;
	end process;

	remain <= r(3) & r(2) & numerator(1 downto 0);

end Behavioral;

