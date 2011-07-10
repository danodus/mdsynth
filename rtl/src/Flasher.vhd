--===========================================================================--
--                                                                           --
--                    LED Flasher                                            --
--                                                                           --
--===========================================================================--
--
--  File name      : flasher.vhd
--
--  Entity name    : flasher
--
--  Purpose        : Implements a long counter used to flash a LED
--                   to indicate code has loaded correctly
--
--  Dependencies   : ieee.std_logic_1164
--                   ieee.numeric_std
--                   ieee.std_logic_unsigned
--
--  Author         : John E. Kent
--
--  Email          : dilbert57@opencores.org      
--
--  Web            : http://opencores.org/project,system09
--
--
--  Copyright (C) 2010 John Kent
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
-- Version Author        Date         Changes
--
-- 0.1     John Kent     2010-08-28   Made separate module
--

library ieee;
  use ieee.std_logic_1164.all;
  use ieee.numeric_std.all;
  use ieee.std_logic_unsigned.all;
--library unisim;
--  use unisim.vcomponents.all;

-----------------------------------------------------------------------
--                    Entity for B3_SRAM                             --
-----------------------------------------------------------------------

entity flasher is
  port (
    clk      : in  std_logic;           -- Clock input
    rst      : in  std_logic;           -- Reset input (active high)
    LED      : out Std_Logic            -- LED output        
  );
end flasher;

--================== End of entity ==============================--

-------------------------------------------------------------------------------
-- Architecture for Flasher
-------------------------------------------------------------------------------

architecture rtl of  flasher is

-- Flashing LED test signals
signal countL       : std_logic_vector(23 downto 0);

begin

--
-- LED Flasher to indicate code has loaded
--
my_LED_Flasher : process (clk, rst, CountL )
begin
  if falling_edge(clk) then
    if rst = '1' then
      countL <= (others=>'0');
    else
      countL <= countL + 1;			 
    end if;
  end if;
  LED <= countL(23);
end process;

end rtl;