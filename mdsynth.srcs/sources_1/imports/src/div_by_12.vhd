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
    Port ( numerator : in  unsigned(6 downto 0);
           quotient : out  unsigned(3 downto 0);
           remain : out  unsigned(3 downto 0));
end div_by_12;

architecture div_by_12_arch of div_by_12 is
signal r: unsigned(1 downto 0);
begin

	-- We divide by 3 on the remaining bits
	process (numerator(6 downto 2))
	begin
		case numerator(6 downto 2) is
			when "00000" => quotient <= "0000"; r <= "00";
			when "00001" => quotient <= "0000"; r <= "01";
			when "00010" => quotient <= "0000"; r <= "10";
			when "00011" => quotient <= "0001"; r <= "00";
			when "00100" => quotient <= "0001"; r <= "01";
			when "00101" => quotient <= "0001"; r <= "10";
			when "00110" => quotient <= "0010"; r <= "00";
			when "00111" => quotient <= "0010"; r <= "01";
			when "01000" => quotient <= "0010"; r <= "10";
			when "01001" => quotient <= "0011"; r <= "00";
			when "01010" => quotient <= "0011"; r <= "01";
			when "01011" => quotient <= "0011"; r <= "10";
			when "01100" => quotient <= "0100"; r <= "00";
			when "01101" => quotient <= "0100"; r <= "01";
			when "01110" => quotient <= "0100"; r <= "10";
			when "01111" => quotient <= "0101"; r <= "00";
			when "10000" => quotient <= "0101"; r <= "01";
			when "10001" => quotient <= "0101"; r <= "10";
			when "10010" => quotient <= "0110"; r <= "00";
			when "10011" => quotient <= "0110"; r <= "01";
			when "10100" => quotient <= "0110"; r <= "10";
			when "10101" => quotient <= "0111"; r <= "00";
			when "10110" => quotient <= "0111"; r <= "01";
			when "10111" => quotient <= "0111"; r <= "10";
			when "11000" => quotient <= "1000"; r <= "00";
			when "11001" => quotient <= "1000"; r <= "01";
			when "11010" => quotient <= "1000"; r <= "10";
			when "11011" => quotient <= "1001"; r <= "00";
			when "11100" => quotient <= "1001"; r <= "01";
			when "11101" => quotient <= "1001"; r <= "10";
			when "11110" => quotient <= "1010"; r <= "00";
			when "11111" => quotient <= "1010"; r <= "01";
			
			when others => quotient <= "0000"; r <= "00";    -- Should not happen
		end case;
	end process;

	remain <= r(1) & r(0) & numerator(1 downto 0);

end div_by_12_arch;

