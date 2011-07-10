--===========================================================================--
--                                                                           --
--  ACIA_Clock.vhd - Synthesizable Baud Rate Clock Divider                   --
--                                                                           --
--===========================================================================--
--
--  File name      : ACIA_Clock.vhd
--
--  Purpose        : Implements a baud rate clock divider for a 6850 compatible
--                   Asynchronous Communications Interface Adapter 
--                  
--  Dependencies   : ieee.std_logic_1164
--                   ieee.std_logic_arith
--                   ieee.std_logic_unsigned
--                   ieee.numeric_std
--                   work.bit_funcs
--
--  Author         : John E. Kent
--
--  Email          : dilbert57@opencores.org      
--
--  Web            : http://opencores.org/project,system09
--
--  ACIA_Clock.vhd is baud rate clock divider for a 6850 compatible ACIA core.
-- 
--  Copyright (C) 2003 - 2010 John Kent
--
--  This program is free software: you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation, either version 3 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program.  If not, see <http://www.gnu.org/licenses/>.
--
--===========================================================================--
--                                                                           --
--                              Revision  History                            --
--                                                                           --
--===========================================================================--
--
-- Revision Name          Date             Description
-- 0.1      John Kent     unknown          Initial version
-- 1.0      John Kent     30th May 2010    Added GPL header 
--      

library ieee;
   use ieee.std_logic_1164.all;
   use ieee.std_logic_arith.all;
   use ieee.std_logic_unsigned.all;
   use ieee.numeric_std.all;
--library unisim;
--	use unisim.vcomponents.all;
library work;
   use work.bit_funcs.all;

entity acia_clock is
  generic (
     SYS_CLK_FREQ  : integer;
	  ACIA_CLK_FREQ : integer
  );   
  port(
    clk      : in  Std_Logic;  -- System Clock input
	 acia_clk : out Std_Logic   -- ACIA Clock output
  );
end acia_clock;

-------------------------------------------------------------------------------
-- Architecture for ACIA_Clock
-------------------------------------------------------------------------------
architecture rtl of ACIA_Clock is

constant FULL_CYCLE : integer :=  (SYS_CLK_FREQ / ACIA_CLK_FREQ);
constant HALF_CYCLE : integer :=  (FULL_CYCLE / 2);
signal   acia_count : Std_Logic_Vector(log2(FULL_CYCLE) downto 0) := (Others => '0');

begin
--
-- Baud Rate Clock Divider
--
-- 25MHz / 27  = 926,000 KHz = 57,870Bd * 16
-- 50MHz / 54  = 926,000 KHz = 57,870Bd * 16
--
my_acia_clock: process( clk  )
begin
    if(clk'event and clk = '0') then
		if( acia_count = (FULL_CYCLE - 1) )	then
			acia_clk   <= '0';
		   acia_count <= (others => '0'); --"000000";
		else
		   if( acia_count = (HALF_CYCLE - 1) )	then
				acia_clk <='1';
			end if;
		   acia_count <= acia_count + 1; 
		end if;			 
    end if;
end process;

end rtl;
