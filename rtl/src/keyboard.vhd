--===========================================================================--
--                                                                           --
--  keyboard.vhd - Synthesizable Interface to PS/2 Keyboard Module           --
--                                                                           --
--===========================================================================--
--
--  File name      : keyboard.vhd
--
--  Entity name    : keyboard
--
--  Purpose        : Implements a CPU interface to John Clayton's PS/2 Keyboard
--                  
--  Dependencies   : ieee.std_logic_1164
--                   ieee.std_logic_arith
--                   ieee.std_logic_unsigned
--                   ieee.numeric_std
--
--  Uses           : ps2_keyboard_interface
--
--  Author         : John E. Kent
--
--  Email          : dilbert57@opencores.org      
--
--  Web            : http://opencores.org/project,system09
--
--  Registers      :
--
--  IO address + 0 read - Status Register
--
--      Bit[0] - RX Data Ready
--      Bit[1] - TX Data Empty
--      Bit[2] - Extended
--      Bit[3] - Released
--      Bit[4] - Shift On
--      Bit[5] - Error (no keyboard acknowledge received)
--      Bit[6] - TX Interrupt Flag
--      Bit[7] - RX Interrupt Flag
--
--  IO address + 0 write - Control Register
--
--      Bit[0] - Undefined
--      Bit[1] - Undefined
--      Bit[2] - Undefined
--      Bit[3] - Undefined
--      Bit[4] - Undefined
--      Bit[5] - Undefined
--      Bit[6] - TX Interrupt Enable
--      Bit[7] - RX Interrupt Enable
--
--  IO address + 1 read - Receive Data Register
--
--  IO address + 1 write - Transmit Data Register
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
-- Version  Author     Date               Description
-- 0.1      John Kent  2nd April 2004     Interface to John Clayton's PS2 keyboard
-- 0.2      John Kent  7th Februaury 2007 Added Generics for Keyboard Timing
-- 0.3      John Kent  30th May 2010      Updated Header, added unisim library
-- 0.4      John Kent  17th June 2010     Cleaned up signal names, 
--                                        added TX interrupt enable
--                                        Modified assignment of status register
--                                        Revised hanshake control and input registers
--                                        

library ieee;
   use ieee.std_logic_1164.all;
   use ieee.std_logic_arith.all;
   use ieee.std_logic_unsigned.all;
   use ieee.numeric_std.all;
--library unisim;
--   use unisim.vcomponents.all;

entity keyboard is
  generic (
  KBD_CLK_FREQ : integer
  );

  port(
  --
  -- CPU Interface Signals
  --
  clk             : in    std_logic;
  rst             : in    std_logic;
  cs              : in    std_logic;
  addr            : in    std_logic;
  rw              : in    std_logic;
  data_in         : in    std_logic_vector(7 downto 0);
  data_out        : out   std_logic_vector(7 downto 0);
  irq             : out   std_logic;
  --
  -- Keyboard Interface Signals
  --
  kbd_clk         : inout std_logic;
  kbd_data        : inout std_logic
  );
end keyboard;

architecture rtl of keyboard is

constant CLK_FREQ_MHZ : integer := KBD_CLK_FREQ / 1000000;

signal kbd_status     : std_logic_vector(7 downto 0);
signal kbd_control    : std_logic_vector(7 downto 0);
signal kbd_rx_data    : std_logic_vector(7 downto 0);
signal kbd_tx_data    : std_logic_vector(7 downto 0);
signal kbd_read       : std_logic;
signal kbd_write      : std_logic;
signal kbd_data_ready : std_logic; --  => kbd_status(0)
signal kbd_data_empty : std_logic; --  => kbd_status(1)
signal kbd_extended   : std_logic; --  => kbd_status(2)
signal kbd_released   : std_logic; --  => kbd_status(3)
signal kbd_shift_on   : std_logic; --  => kbd_status(4)
signal kbd_error      : std_logic; --  => kbd_status(5) (No receive acknowledge)

component ps2_keyboard
  generic (
  CLK_FREQ_MHZ    : integer := CLK_FREQ_MHZ
  );

  port(
  clk             : in    std_logic;
  reset           : in    std_logic;
  rx_data         : out   std_logic_vector(7 downto 0);
  rx_read         : in    std_logic;
  rx_data_ready   : out   std_logic;
  rx_extended     : out   std_logic;
  rx_released     : out   std_logic;
  rx_shift_on     : out   std_logic;
  tx_data         : in    std_logic_vector(7 downto 0);
  tx_write        : in    std_logic;
  tx_data_empty   : out   std_logic;
  tx_error        : out   std_logic;
  ps2_clk         : inout std_logic;
  ps2_data        : inout std_logic
  );
end component;

begin

my_ps2_keyboard : ps2_keyboard
  generic map (
  CLK_FREQ_MHZ   => CLK_FREQ_MHz
  )

  port map(
  clk             => clk,
  reset           => rst,
  rx_data         => kbd_rx_data,
  rx_read         => kbd_read,
  rx_data_ready   => kbd_data_ready,
  rx_extended     => kbd_extended,
  rx_released     => kbd_released,
  rx_shift_on     => kbd_shift_on,
  tx_data         => kbd_tx_data,
  tx_write        => kbd_write,
  tx_data_empty   => kbd_data_empty,
  tx_error        => kbd_error,
  ps2_clk         => kbd_clk,
  ps2_data        => kbd_data
  );

  --
  -- Keyboard Read strobe
  --
  keyboard_read : process( clk, rst, cs, rw, kbd_data_ready  )
  begin
    if rst = '1' then
      kbd_read <= '0';
    elsif( clk'event and clk='0' ) then
      if( cs = '1' and addr = '1' and rw = '1' ) then
        kbd_read <= '1';
      elsif kbd_data_ready = '1' then
        kbd_read <= '0';
      end if;
    end if;
  end process;

  --
  -- Keyboard Write strobe
  --
  keyboard_write : process( clk, rst, cs, rw, addr,
									 kbd_write, kbd_data_empty )
  begin
    if rst = '1' then
		kbd_write   <= '0';
    elsif( clk'event and clk='0' ) then
		if( cs = '1' and addr = '1' and rw = '0' ) then
         kbd_write <= '1';
      elsif kbd_data_empty = '1' then
         kbd_write <= '0';
      end if;
    end if;
  end process;

  --
  -- Keyboard register input
  --
  keyboard_in : process( clk, rst, cs, rw, addr, data_in, kbd_rx_data )
  begin
    if rst = '1' then
		kbd_control <= (others => '0');
      kbd_tx_data <= (others => '0');
    elsif( clk'event and clk='0' ) then
		if( cs = '1' and rw = '0' ) then
         if addr = '0' then
            kbd_control <= data_in;
         else
            kbd_tx_data <= data_in;
         end if;
      end if;
    end if;
  end process;

  --
  -- Keyboard register output
  --
  keyboard_out : process( addr, kbd_rx_data, kbd_status )
  begin
    if( addr = '0' ) then
      data_out <= kbd_status;
    else
      data_out <= kbd_rx_data;
    end if; 
  end process;

  --
  -- Assign Keyboard Status bits
  --
  keyboard_status : process( kbd_data_ready, kbd_data_empty,
                             kbd_extended, kbd_released, kbd_shift_on, kbd_error,
                             kbd_control, kbd_status )
  begin
    kbd_status(0) <= kbd_data_ready;
    kbd_status(1) <= kbd_data_empty;
    kbd_status(2) <= kbd_extended;
    kbd_status(3) <= kbd_released;
    kbd_status(4) <= kbd_shift_on;
    kbd_status(5) <= kbd_error;
    kbd_status(6) <= kbd_control(6) and kbd_data_empty; -- TX Interrupt Flag
    kbd_status(7) <= kbd_control(7) and kbd_data_ready; -- RX Interrupt Flag
    irq           <= kbd_status(7) or kbd_status(6);
  end process;

end rtl;

