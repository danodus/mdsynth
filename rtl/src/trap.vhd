--===========================================================================--
--                                                                           --
--             Synthesizable Hardware Breakpoint Trap                        --
--                                                                           --
--===========================================================================--
--
--  File name      : trap.vhd
--
--  Entity name    : trap
--
--  Purpose        : Implements a 8 bit address and data hardware breakpoint comparator
--                   which generates an interrupt output on qualified match conditions
--
--  Dependencies   : ieee.Std_Logic_1164
--                   ieee.std_logic_unsigned
--
--  Author         : John E. Kent      
--
--  Email          : dilbert57@opencores.org      
--
--  Web            : http://opencores.org/project,system09
-- 
--  Description    : Register Memory Map
--
--                   Base + $00 - Address Comparitor High Byte
--                   Base + $01 - Address Comparitor Low byte
--                   Base + $02 - Data    Comparitor
--                   Base + $03 - Control Comparitor
--                   Base + $04 - Address Qualifier High Byte
--                   Base + $05 - Address Qualifier Low byte
--                   Base + $06 - Data    Qualifier
--                   Base + $07 - Control Qualifier
--
--                   Address, Data and Control signals 
--                   must match in the Comparitor registers 
--                   Matches are qualified by setting a bit 
--                   in the Qualifier registers
--
--                   Control Comparitor / Control Qualify (write)
--                   b0 - r/w        1=read  0=write
--                   b1 - vma        1=valid 0=invalid
--                   b7 - irq output 1=match 0=mismatch
--
--                   Control Qualifier Read
--                   b7 - match flag
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
--                             Revision History                              --
--                                                                           --
--===========================================================================--
-- Version Author     Date        Description
-- 0.1     John Kent  2003-05-05  Initial version
-- 0.2     John kent  2010-08-09  Updated header & GPL information
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity trap is
	port (	
	 clk        : in  std_logic;
    rst        : in  std_logic;
    cs         : in  std_logic;
    rw         : in  std_logic;
	 vma        : in  std_logic;
    addr       : in  std_logic_vector(15 downto 0);
    data_in    : in  std_logic_vector(7 downto 0);
	 data_out   : out std_logic_vector(7 downto 0);
	 irq        : out std_logic
  );
end;

architecture trap_arch of trap is

--
-- Trap registers
--
signal comp_addr_hi : std_logic_vector(7 downto 0);
signal comp_addr_lo : std_logic_vector(7 downto 0);
signal qual_addr_hi : std_logic_vector(7 downto 0);
signal qual_addr_lo : std_logic_vector(7 downto 0);
signal comp_data    : std_logic_vector(7 downto 0);
signal qual_data    : std_logic_vector(7 downto 0);
signal comp_ctrl    : std_logic_vector(7 downto 0);
signal qual_ctrl    : std_logic_vector(7 downto 0);
signal match_flag   : std_logic;
begin


--------------------------------
--
-- write control registers
--
--------------------------------
trap_write : process( clk, rst, cs, rw, addr, data_in,
                      comp_addr_hi, comp_addr_lo, comp_data, comp_ctrl,
                      qual_addr_hi, qual_addr_lo, qual_data, qual_ctrl )
begin
  if clk'event and clk = '0' then
    if rst = '1' then
		  comp_addr_hi <= "00000000";
		  comp_addr_lo <= "00000000";
		  comp_data    <= "00000000";
		  comp_ctrl    <= "00000000";
		  qual_addr_hi <= "00000000";
		  qual_addr_lo <= "00000000";
		  qual_data    <= "00000000";
		  qual_ctrl    <= "00000000";
    elsif cs = '1' and rw = '0' then
	   case addr(2 downto 0) is
		when "000" =>
		  comp_addr_hi <= data_in;
		  comp_addr_lo <= comp_addr_lo;
		  comp_data    <= comp_data;
		  comp_ctrl    <= comp_ctrl;
		  qual_addr_hi <= qual_addr_hi;
		  qual_addr_lo <= qual_addr_lo;
		  qual_data    <= qual_data;
		  qual_ctrl    <= qual_ctrl;
		when "001" =>
		  comp_addr_hi <= comp_addr_hi;
		  comp_addr_lo <= data_in;
		  comp_data    <= comp_data;
		  comp_ctrl    <= comp_ctrl;
		  qual_addr_hi <= qual_addr_hi;
		  qual_addr_lo <= qual_addr_lo;
		  qual_data    <= qual_data;
		  qual_ctrl    <= qual_ctrl;
		when "010" =>
		  comp_addr_hi <= comp_addr_hi;
		  comp_addr_lo <= comp_addr_lo;
		  comp_data    <= data_in;
		  comp_ctrl    <= comp_ctrl;
		  qual_addr_hi <= qual_addr_hi;
		  qual_addr_lo <= qual_addr_lo;
		  qual_data    <= qual_data;
		  qual_ctrl    <= qual_ctrl;
		when "011" =>
		  comp_addr_hi <= comp_addr_hi;
		  comp_addr_lo <= comp_addr_lo;
		  comp_data    <= comp_data;
		  comp_ctrl    <= data_in;
		  qual_addr_hi <= qual_addr_hi;
		  qual_addr_lo <= qual_addr_lo;
		  qual_data    <= qual_data;
		  qual_ctrl    <= qual_ctrl;
		when "100" =>
		  comp_addr_hi <= comp_addr_hi;
		  comp_addr_lo <= comp_addr_lo;
		  comp_data    <= comp_data;
		  comp_ctrl    <= comp_ctrl;
		  qual_addr_hi <= data_in;
		  qual_addr_lo <= qual_addr_lo;
		  qual_data    <= qual_data;
		  qual_ctrl    <= qual_ctrl;
		when "101" =>
		  comp_addr_hi <= comp_addr_hi;
		  comp_addr_lo <= comp_addr_lo;
		  comp_data    <= comp_data;
		  comp_ctrl    <= comp_ctrl;
		  qual_addr_hi <= qual_addr_hi;
		  qual_addr_lo <= data_in;
		  qual_data    <= qual_data;
		  qual_ctrl    <= qual_ctrl;
		when "110" =>
		  comp_addr_hi <= comp_addr_hi;
		  comp_addr_lo <= comp_addr_lo;
		  comp_data    <= comp_data;
		  comp_ctrl    <= comp_ctrl;
		  qual_addr_hi <= qual_addr_hi;
		  qual_addr_lo <= qual_addr_lo;
		  qual_data    <= data_in;
		  qual_ctrl    <= qual_ctrl;
--  		when "111" =>
      when others =>
		  comp_addr_hi <= comp_addr_hi;
		  comp_addr_lo <= comp_addr_lo;
		  comp_data    <= comp_data;
		  comp_ctrl    <= comp_ctrl;
		  qual_addr_hi <= qual_addr_hi;
		  qual_addr_lo <= qual_addr_lo;
		  qual_data    <= qual_data;
		  qual_ctrl    <= data_in;
		end case;
	 else
		  comp_addr_hi <= comp_addr_hi;
		  comp_addr_lo <= comp_addr_lo;
		  comp_data    <= comp_data;
		  comp_ctrl    <= comp_ctrl;
		  qual_addr_hi <= qual_addr_hi;
		  qual_addr_lo <= qual_addr_lo;
		  qual_data    <= qual_data;
		  qual_ctrl    <= qual_ctrl;
	 end if;
  end if;
end process;

--
-- trap data output mux
--
trap_read : process( addr,
                     comp_addr_hi, comp_addr_lo, comp_data, comp_ctrl,
                     qual_addr_hi, qual_addr_lo, qual_data, qual_ctrl,
							match_flag )
begin
   case addr(2 downto 0) is
	when "000" =>
	   data_out <= comp_addr_hi;
	when "001" =>
	   data_out <= comp_addr_lo;
	when "010" =>
	   data_out <= comp_data;
	when "011" =>
	   data_out <= comp_ctrl;
	when "100" =>
	   data_out <= qual_addr_hi;
	when "101" =>
	   data_out <= qual_addr_lo;
	when "110" =>
	   data_out <= qual_data;
--	when "111" =>
   when others =>
	   data_out(6 downto 0) <= qual_ctrl(6 downto 0);
		data_out(7) <= match_flag;
	end case;
end process;


--
-- Trap hardware
--
trap_match : process( Clk, rst, cs, rw, addr, vma, match_flag, data_in,
                      comp_addr_hi, comp_addr_lo, comp_data, comp_ctrl,
							 qual_addr_hi, qual_addr_lo, qual_data, qual_ctrl)
variable match         : std_logic;
variable match_addr_hi : std_logic;
variable match_addr_lo : std_logic;
variable match_data    : std_logic;
variable match_ctrl    : std_logic;

begin
  match_addr_hi := 
           ((comp_addr_hi(7) xor addr(15)  ) and qual_addr_hi(7) ) or
		     ((comp_addr_hi(6) xor addr(14)  ) and qual_addr_hi(6) ) or
		     ((comp_addr_hi(5) xor addr(13)  ) and qual_addr_hi(5) ) or
		     ((comp_addr_hi(4) xor addr(12)  ) and qual_addr_hi(4) ) or
		     ((comp_addr_hi(3) xor addr(11)  ) and qual_addr_hi(3) ) or
		     ((comp_addr_hi(2) xor addr(10)  ) and qual_addr_hi(2) ) or
		     ((comp_addr_hi(1) xor addr( 9)  ) and qual_addr_hi(1) ) or
		     ((comp_addr_hi(0) xor addr( 8)  ) and qual_addr_hi(0) );
  match_addr_lo :=
		     ((comp_addr_lo(7) xor addr( 7)  ) and qual_addr_lo(7) ) or
		     ((comp_addr_lo(6) xor addr( 6)  ) and qual_addr_lo(6) ) or
		     ((comp_addr_lo(5) xor addr( 5)  ) and qual_addr_lo(5) ) or
		     ((comp_addr_lo(4) xor addr( 4)  ) and qual_addr_lo(4) ) or
		     ((comp_addr_lo(3) xor addr( 3)  ) and qual_addr_lo(3) ) or
		     ((comp_addr_lo(2) xor addr( 2)  ) and qual_addr_lo(2) ) or
		     ((comp_addr_lo(1) xor addr( 1)  ) and qual_addr_lo(1) ) or
		     ((comp_addr_lo(0) xor addr( 0)  ) and qual_addr_lo(0) );
  match_data :=
           ((comp_data(7)    xor data_in(7)) and qual_data(7)    ) or
           ((comp_data(6)    xor data_in(6)) and qual_data(6)    ) or
           ((comp_data(5)    xor data_in(5)) and qual_data(5)    ) or
           ((comp_data(4)    xor data_in(4)) and qual_data(4)    ) or
           ((comp_data(3)    xor data_in(3)) and qual_data(3)    ) or
           ((comp_data(2)    xor data_in(2)) and qual_data(2)    ) or
           ((comp_data(1)    xor data_in(1)) and qual_data(1)    ) or
           ((comp_data(0)    xor data_in(0)) and qual_data(0)    );
  match_ctrl :=
           ((comp_ctrl(0)    xor rw        ) and qual_ctrl(0)    ) or
           ((comp_ctrl(1)    xor vma       ) and qual_ctrl(1)    );

  match := not ( match_addr_hi or match_addr_lo or match_data or match_ctrl);

	 if rst = '1' then
		match_flag <= '0';
    elsif clk'event and clk = '0' then
      if cs = '1' and rw = '0' then
		  match_flag <= '0';
      else
		  if match = comp_ctrl(7) then
		    match_flag <= '1';
		  end if;
		end if;
    end if; 
	 irq <= match_flag and qual_ctrl(7);
  end process;

end trap_arch;
	
