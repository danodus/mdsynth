--===========================================================================--
--                                                                           --
--               Synthesizable PS/2 Keyboard Key map ROM                     --
--                                                                           --
--===========================================================================--
--
--  File name      : keymap_rom_slice.vhd
--
--  Entity name    : keymap_rom 
--
--  Purpose        : PS/2 key code look up table for PS/2 Keyboard
--                   Converts 7 bit key code to ASCII
--                   Address bit 8      = Shift
--                   Address bit 7      = CAPS Lock
--                   Address bits 6 - 0 = Key code
--                   Data bits 6 - 0    = ASCII code
--                   Using constant array look up.
--
--  Dependencies   : ieee.std_logic_1164
--                   ieee.std_logic_arith
--                   ieee.std_logic_unsigned
--
--  Uses           : None
--
--  Author         : John E. Kent
--
--  Email          : dilbert57@opencores.org      
--
--  Web            : http://opencores.org/project,system09
--
--  Copyright (C) 2004 - 2010 John Kent
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
-- Version Date        Author     Changes
--
-- 0.1     2004-10-18  John Kent  Initial version
--
-- 0.2     2007-01-28  John Kent  Made entity compatible with block RAM versions
--
-- 0.3     2007-02-03  John Kent  Initialized with bit_vector
--
-- 0.4     2010-06-17  John Kent  Updated header and added GPL
--                                Renamed data_in and data_out signals
--
--
library IEEE;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity keymap_rom is
    Port (
       clk      : in  std_logic;
       rst      : in  std_logic;
       cs       : in  std_logic;
       rw       : in  std_logic;
       addr     : in  std_logic_vector (8 downto 0);
       data_in  : in  std_logic_vector (7 downto 0);
       data_out : out std_logic_vector (7 downto 0)
    );
end keymap_rom;

architecture rtl of keymap_rom is

  type rom_array is array(0 to 15) of std_logic_vector (255 downto 0);

  constant rom_data : rom_array :=
  ( 
    x"00327761737a0000003171000000000000600900000000000000000000000000",	-- 1F - 00
    x"003837756a6d00000036796768626e0000357274667620000033346564786300",	-- 3F - 20
    x"00005c005d0d000000003d5b00270000002d703b6c2f2e000039306f696b2c00",	-- 5F - 40
    x"0000000000000000001b000000007f0000000000000000000008000000000000",	-- 7F - 60

    x"00325741535a00000031510000000000007e0900000000000000000000000000",	-- 9F - 80
    x"003837554a4d00000036594748424e0000355254465620000033344544584300",	-- BF - A0
    x"00005c005d0d000000003d5b00270000002d503b4c2f2e000039304f494b2c00",	-- DF - C0
    x"0000000000000000001b000000007f0000000000000000000008000000000000",	-- FF - E0

    x"00405741535a00000021510000000000007e0900000000000000000000000000",	-- 1F - 00
    x"002a26554a4d0000005e594748424e0000255254465620000023244544584300",	-- 3F - 20
    x"00007c007d0d000000002b7b00220000005f503a4c3f3e000028294f494b3c00",	-- 5F - 40
    x"0000000000000000001b000000007f0000000000000000000008000000000000",	-- 7F - 60

    x"00407761737a0000002171000000000000600900000000000000000000000000",	-- 9F - 80
    x"002a26756a6d0000005e796768626e0000257274667620000023246564786300",	-- BF - A0
    x"00007c007d0d000000002b7b00220000005f703a6c3f3e000028296f696b3c00",	-- DF - C0
    x"0000000000000000001b000000007f0000000000000000000008000000000000"	   -- FF - E0
  );

  signal rom_out : std_logic_vector(255 downto 0);

begin

  process( addr, rom_out )
  begin
    rom_out  <= rom_data(conv_integer(addr(8 downto 5))); 
	 data_out <= rom_out( conv_integer(addr(4 downto 0))*8+7 downto conv_integer(addr(4 downto 0))*8);
  end process;

end architecture rtl;

