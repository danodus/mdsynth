--===========================================================================
--
--  clock_div.vhd - Clock divider for System09
--
--===========================================================================
--
-- File name      : clock_div.vhd
--
-- Entity name    : clock_div
--
-- Purpose        : Generates Clocks for System09
--                  For BurchED B3-Spartan2+ and B5-X300
--                  Divides the input clock which is normally 50MHz
--                  Generates a 1/1 (50.0 MHz) SYS clock 
--                  Generates a 1/2 (25.0 MHz) VGA clock 
--                  Generates a 1/4 (12.5 MHz) CPU clock 
--
-- Dependencies   : ieee.Std_Logic_1164
--                  ieee.std_logic_unsigned
--                  ieee.std_logic_arith
--                  ieee.numeric_std
--
-- Uses           : IBUFG
--                  BUFG
--
-- Author         : John E. Kent      
--                  dilbert57@opencores.org      
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
--===========================================================================
--
--                              Revision History:
--
--===========================================================================
--
-- Rev: Date:       Author:    Description:
--
-- 0.1  2008-09-07  John Kent  Initial version
-- 0.2  2010-09-14  John Kent  Updated header
--
--
library ieee;
   use ieee.std_logic_1164.all;
   use IEEE.STD_LOGIC_ARITH.ALL;
   use IEEE.STD_LOGIC_UNSIGNED.ALL;
   use ieee.numeric_std.all;
--library unisim;
--	use unisim.vcomponents.all;

entity clock_div is
  port(
    clk_in      : in  std_Logic;  -- System Clock input
	 sys_clk     : out std_logic;  -- System Clock Out    (1/1)
	 vga_clk     : out std_logic;  -- VGA Pixel Clock Out (1/2)
    cpu_clk     : out std_logic   -- CPU Clock Out       (1/4)
  );
end entity;

architecture RTL of clock_div is

signal div_count   : std_logic_vector(1 downto 0);

component BUFG 
  port (
		i: in  std_logic;
		o: out std_logic
  );
end component;

--
-- Start instantiation
--
begin

--
-- 50 MHz SYS clock output
--
sys_clk_buffer : BUFG 
  port map(
    i => clk_in,
	 o => sys_clk
  );

--
-- 25 MHz VGA clock output
--
vga_clk_buffer : BUFG 
  port map(
    i => div_count(0),
	 o => vga_clk
  );

--
-- 12.5MHz CPU clock 
--
cpu_clk_buffer : BUFG 
  port map(
    i => div_count(1),
	 o => cpu_clk
  );

--
-- Clock divider
--
clock_div : process( clk_in )
begin
  if rising_edge( clk_in ) then
    div_count <= div_count + "01";
  end if;
end process;

end architecture;
 