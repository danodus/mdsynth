--===========================================================================--
--                                                                           --
--                  Synthesizable 8 bit Timer                                --
--                                                                           --
--===========================================================================--
--
--  File name      : timer.vhd
--
--  Entity name    : timer
--
--  Purpose        : 8 bit timer module for System09
--
--  Dependencies   : ieee.std_logic_1164
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
--  Registers      :
--
--  IO address + 0 Read - Down Count register
--        Bits[7..0] = Counter Value
--
--  IO address + 0 Write - Preset Count register
--        Bits[7..0] = Preset Value
--
--  IO address + 1 Read - Status register
--        Bit[7]     = Interrupt Flag
--        Bits[6..0] = undefined
--
--  IO address + 1 Write - Control register
--        Bit[7]     = Interrupt Enable
--        Bits[6..1] = Unedfined
--        Bit[0]     = Counter enable
--
--  Operation       :
--
--        Write count to counter register
--        Enable counter by setting bit 0 of the control register
--        Enable interrupts by setting bit 7 of the control register
--        Counter will count down to zero
--        When it reaches zero the terminal flag is set
--        If the interrupt is enabled an interrupt is generated
--        The interrupt may be disabled by writing a 0 to bit 7 
--        of the control register or by loading a new down count 
--        into the counter register.
--
--  Copyright (C) 2002 - 2010 John Kent
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
-- 0.1     2002-09-06  John Kent  Converted to a single timer 
--                                Made synchronous with system clock
-- 1.0     2003-09-06  John Kent  Changed Clock Edge
--                                Released to opencores.org
-- 2.0     2008-02-05  John Kent  Removed Timer inputs and outputs
--                                Made into a simple 8 bit interrupt down counter
-- 2.1     2010-06-17  John Kent  Updated header and added GPL
--
-- 
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity timer is
	port (	
	 clk        : in  std_logic;
    rst        : in  std_logic;
    cs         : in  std_logic;
    addr       : in  std_logic;
    rw         : in  std_logic;
    data_in    : in  std_logic_vector(7 downto 0);
	 data_out   : out std_logic_vector(7 downto 0);
	 irq        : out std_logic
  );
end;

architecture rtl of timer is
signal timer_ctrl  : std_logic_vector(7 downto 0);
signal timer_stat  : std_logic_vector(7 downto 0);
signal timer_count : std_logic_vector(7 downto 0);
signal timer_term  : std_logic; -- Timer terminal count
--
-- control/status register bits
--
constant BIT_ENB   : integer := 0; -- 0=disable, 1=enabled
constant BIT_IRQ   : integer := 7; -- 0=disabled, 1-enabled

begin

  --------------------------------
  --
  -- write control registers
  --
  --------------------------------
  timer_control : process( clk, rst, cs, rw, addr, data_in,
                         timer_ctrl, timer_term, timer_count )
  begin
    if clk'event and clk = '0' then
      if rst = '1' then
	     timer_count <= (others=>'0');
		  timer_ctrl  <= (others=>'0');
		  timer_term  <= '0';
      elsif cs = '1' and rw = '0' then
	     if addr='0' then
		    timer_count <= data_in;
		    timer_term  <= '0';
	     else
		    timer_ctrl <= data_in;
		  end if;
	   else
	     if (timer_ctrl(BIT_ENB) = '1') then
		    if (timer_count = "00000000" ) then
		      timer_term <= '1';
          else
            timer_count <= timer_count - 1;
		    end if;
		  end if;
      end if;
    end if;
  end process;

  --
  -- timer status register
  --
  timer_status : process( timer_ctrl, timer_term )
  begin
    timer_stat(6 downto 0) <= timer_ctrl(6 downto 0);
    timer_stat(BIT_IRQ) <= timer_term;
  end process;

  --
  -- timer data output mux
  --
  timer_data_out : process( addr, timer_count, timer_stat )
  begin
    if addr = '0' then
      data_out <= timer_count;
    else
      data_out <= timer_stat;
    end if;
  end process;

  --
  -- read timer strobe to reset interrupts
  --
  timer_interrupt : process( timer_term, timer_ctrl )
  begin
	 irq <= timer_term and timer_ctrl(BIT_IRQ);
  end process;

end rtl;
	
