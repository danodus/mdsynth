-- MDSynth Top Level
-- 
-- Authors        : John E. Kent (dilbert57@opencores.org)
--                  Daniel Cliche (dcliche@meldora.com)
--
--  Copyright (C) 2003 - 2010 John Kent
--  Copyright (C) 2011 Meldora Inc.
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
-- Memory Map :
--
-- $0000 - $7FFF System Block RAM
-- $E000 - ACIA (SWTPc)
-- $E010 - MIDI ACIA
-- $E020 - Keyboard
-- $E030 - VDU
-- $E040 - Reserved for SWTPc MP-T (was Compact Flash)
-- $E050 - Timer
-- $E060 - Bus Trap (Hardware Breakpoint Interrupt Logic)
-- $E070 - PIA Single Step Timer (was Reserved for Trace Buffer)
-- $E080 - Sound Voice 0
-- $E090 - Sound Voice 1
-- $E0A0 - Sound Voice 2
-- $E0B0 - Sound Voice 3
-- $E0C0 - Reserved
-- $E100 - $E13F Reserved IDE / Compact Flash Card
-- $E140 - $E17F Reserved for Ethernet MAC (XESS)
-- $E180 - $E1BF Reserved for Expansion Slot 0 (XESS)
-- $E1C0 - $E1FF Reserved for Expansion Slot 1 (XESS)
-- $E200 - $EFFF Dual Port RAM interface
-- $F000 - $F7FF Reserved SWTPc DMAF-2
-- $F800 - $FFFF Sys09bug ROM (Read only)
-- $FFF0 - $FFFF Reserved for DAT - Dynamic Address Translation (Write Only)


library ieee;
   use ieee.std_logic_1164.all;
   use IEEE.STD_LOGIC_ARITH.ALL;
   use IEEE.STD_LOGIC_UNSIGNED.ALL;
   use ieee.numeric_std.all;

entity mdsynth is
  port(
    CLK_50MHZ     : in  Std_Logic;  -- System Clock input
    BTN_SOUTH     : in  Std_Logic;

 	 -- PS/2 Keyboard
	 PS2_CLK      : inout Std_logic;
	 PS2_DATA     : inout Std_Logic;

	 -- CRTC output signals
	 VGA_VSYNC    : out Std_Logic;
    VGA_HSYNC     : out Std_Logic;
    VGA_B         : out std_logic_vector(3 downto 0);
    VGA_G         : out std_logic_vector(3 downto 0);
    VGA_R         : out std_logic_vector(3 downto 0);

	 -- Uart Interface
	 RS232_DCE_RXD : in  std_logic;
    RS232_DCE_TXD : out std_logic;
    
    -- MIDI Interface
    MIDI_RS232_DCE_RXD : in  std_logic;
    MIDI_RS232_DCE_TXD : out std_logic;
    	 
	 -- LEDS & Switches
	 LED           : out std_logic_vector(7 downto 0);
	 
	 -- Audio
	 AUD_L        : out std_logic;
	 AUD_R        : out std_logic;
	 AUX_AUD_L    : out std_logic;
	 AUX_AUD_R    : out std_logic
	 );
end mdsynth;

-------------------------------------------------------------------------------
-- Architecture for System09
-------------------------------------------------------------------------------
architecture my_computer of mdsynth is
  -----------------------------------------------------------------------------
  -- constants
  -----------------------------------------------------------------------------
  constant SYS_CLK_FREQ  : integer := 50000000;  -- FPGA System Clock
  constant VGA_CLK_FREQ  : integer := 25000000;  -- VGA Pixel Clock
  constant CPU_CLK_FREQ  : integer := 25000000;  -- CPU Clock
  constant BAUD_RATE     : integer := 57600;	  -- Baud Rate
  constant ACIA_CLK_FREQ : integer := BAUD_RATE * 16;
  constant MIDI_BAUD_RATE : integer := 31250;	  -- MIDI Baud Rate
  constant MIDI_ACIA_CLK_FREQ : integer := MIDI_BAUD_RATE * 16;

  -----------------------------------------------------------------------------
  -- Signals
  -----------------------------------------------------------------------------
  -- Clocks
  signal sys_clk        : std_logic;
  signal vga_clk        : std_logic;

  -- BOOT ROM
  signal rom_cs         : Std_logic;
  signal rom_data_out   : Std_Logic_Vector(7 downto 0);

  -- UART Interface signals
  signal uart_data_out  : Std_Logic_Vector(7 downto 0);  
  signal uart_cs        : Std_Logic;
  signal uart_irq       : Std_Logic;
  signal uart_clk       : Std_Logic;

  -- MIDI UART Interface signals
  signal midi_uart_data_out  : Std_Logic_Vector(7 downto 0);  
  signal midi_uart_cs        : Std_Logic;
  signal midi_uart_irq       : Std_Logic;
  signal midi_uart_clk       : Std_Logic;

  -- timer
  signal timer_data_out : std_logic_vector(7 downto 0);
  signal timer_cs       : std_logic;
  signal timer_irq      : std_logic;

  -- trap
  signal trap_cs        : std_logic;
  signal trap_data_out  : std_logic_vector(7 downto 0);
  signal trap_irq       : std_logic;

  -- PIA Interface signals
  signal pia_data_out   : Std_Logic_Vector(7 downto 0);  
  signal pia_cs         : Std_Logic;
  signal pia_irq_a      : Std_Logic;
  signal pia_irq_b      : Std_Logic;

  -- keyboard port
  signal kbd_data_out   : std_logic_vector(7 downto 0);
  signal kbd_cs         : std_logic;
  signal kbd_irq        : std_logic;

  -- Video Display Unit
  signal vdu_cs         : std_logic;
  signal vdu_data_out   : std_logic_vector(7 downto 0);

  signal vga_blue       : std_logic;
  signal vga_green      : std_logic;
  signal vga_red        : std_logic;

  -- Sound
  signal sound_cs        : std_logic;
  signal sound_data_out   : std_logic_vector(7 downto 0);
  
  -- RAM
  signal ram_cs         : std_logic; -- memory chip select
  signal ram_data_out   : std_logic_vector(7 downto 0);

  -- CPU Interface signals
  signal cpu_rst        : Std_Logic;
  signal cpu_clk        : Std_Logic;
  signal cpu_rw         : std_logic;
  signal cpu_vma        : std_logic;
  signal cpu_halt       : std_logic;
  signal cpu_hold       : std_logic;
  signal cpu_firq       : std_logic;
  signal cpu_irq        : std_logic;
  signal cpu_nmi        : std_logic;
  signal cpu_addr       : std_logic_vector(15 downto 0);
  signal cpu_data_in    : std_logic_vector(7 downto 0);
  signal cpu_data_out   : std_logic_vector(7 downto 0);

  signal audio_left     : std_logic;
  signal audio_right    : std_logic;
  
-----------------------------------------------------------------
--
--                     Clock generator
--
-----------------------------------------------------------------

component clock_div
  port(
    clk_in      : in  std_Logic;  -- System Clock input
	 sys_clk     : out std_logic;  -- System Clock Out    (1/1)
	 vga_clk     : out std_logic;  -- VGA Pixel Clock Out (1/2)
    cpu_clk     : out std_logic   -- CPU Clock Out       (1/4)
  );
end component;

-----------------------------------------------------------------
--
--                      LED Flasher
--
-----------------------------------------------------------------

component flasher
  port (
    clk      : in  std_logic;           -- Clock input
    rst      : in  std_logic;           -- Reset input (active high)
    LED      : out Std_Logic            -- LED output        
  );
end component;
  
-----------------------------------------------------------------
--
-- CPU09 CPU core
--
-----------------------------------------------------------------

component cpu09
  port (    
	 clk      :	 in std_logic;
    rst      :  in std_logic;
    vma      :	out std_logic;
    addr     : out std_logic_vector(15 downto 0);
    rw       :	out std_logic;
	 data_out : out std_logic_vector(7 downto 0);
    data_in  :  in std_logic_vector(7 downto 0);
	 irq      :  in std_logic;
	 nmi      :  in std_logic;
	 firq     :  in std_logic;
	 halt     :  in std_logic;
	 hold     :  in std_logic
  );
end component;


----------------------------------------
--
-- Block RAM Monitor ROM
--
----------------------------------------
component mon_rom
    Port (
       clk      : in  std_logic;
		 rst      : in  std_logic;
		 cs       : in  std_logic;
		 rw       : in  std_logic;
       addr     : in  std_logic_vector (10 downto 0);
       data_in  : in  std_logic_vector (7 downto 0);
       data_out : out std_logic_vector (7 downto 0)
    );
end component;

----------------------------------------
--
-- Block RAM Monitor
--
----------------------------------------
component ram_32k
    Port (
       clk      : in  std_logic;
		 rst      : in  std_logic;
		 cs       : in  std_logic;
       addr     : in  std_logic_vector (14 downto 0);
		 rw       : in  std_logic;
       data_in  : in  std_logic_vector (7 downto 0);
       data_out : out std_logic_vector (7 downto 0)
    );
end component;

-----------------------------------------------------------------
--
-- 6822 compatible PIA with counters
--
-----------------------------------------------------------------

component pia_timer
	port (	
	 clk       : in    std_logic;
    rst       : in    std_logic;
    cs        : in    std_logic;
    addr      : in    std_logic_vector(1 downto 0);
    rw        : in    std_logic;
    data_in   : in    std_logic_vector(7 downto 0);
	 data_out  : out   std_logic_vector(7 downto 0);
	 irqa      : out   std_logic;
	 irqb      : out   std_logic
	 );
end component;


-----------------------------------------------------------------
--
-- 6850 ACIA/UART
--
-----------------------------------------------------------------

component acia6850
  port (
     clk      : in  Std_Logic;  -- System Clock
     rst      : in  Std_Logic;  -- Reset input (active high)
     cs       : in  Std_Logic;  -- miniUART Chip Select
     rw       : in  Std_Logic;  -- Read / Not Write
     addr     : in  Std_Logic;  -- Register Select
     data_in  : in  Std_Logic_Vector(7 downto 0); -- Data Bus In 
     data_out : out Std_Logic_Vector(7 downto 0); -- Data Bus Out
     irq      : out Std_Logic;  -- Interrupt
     RxC      : in  Std_Logic;  -- Receive Baud Clock
     TxC      : in  Std_Logic;  -- Transmit Baud Clock
     RxD      : in  Std_Logic;  -- Receive Data
     TxD      : out Std_Logic;  -- Transmit Data
	  DCD_n    : in  Std_Logic;  -- Data Carrier Detect
     CTS_n    : in  Std_Logic;  -- Clear To Send
     RTS_n    : out Std_Logic );  -- Request To send
end component;

-----------------------------------------------------------------
--
-- ACIA Clock divider
--
-----------------------------------------------------------------

component ACIA_Clock
  generic (
     SYS_CLK_FREQ  : integer :=  SYS_CLK_FREQ;
	  ACIA_CLK_FREQ : integer := ACIA_CLK_FREQ
  );   
  port (
     clk      : in  Std_Logic;  -- System Clock Input
	  ACIA_clk : out Std_logic   -- ACIA Clock output
  );
end component;

----------------------------------------
--
-- Timer module
--
----------------------------------------

component timer
  port (
     clk       : in  std_logic;
     rst       : in  std_logic;
     cs        : in  std_logic;
     addr      : in  std_logic;
     rw        : in  std_logic;
     data_in   : in  std_logic_vector(7 downto 0);
	  data_out  : out std_logic_vector(7 downto 0);
	  irq       : out std_logic 
	  );
end component;

------------------------------------------------------------
--
-- Bus Trap logic
--
------------------------------------------------------------

component trap
	port (	
	 clk        : in  std_logic;
    rst        : in  std_logic;
    cs         : in  std_logic;
    vma        : in  std_logic;
    rw         : in  std_logic;
    addr       : in  std_logic_vector(15 downto 0);
    data_in    : in  std_logic_vector(7 downto 0);
	 data_out   : out std_logic_vector(7 downto 0);
	 irq        : out std_logic
  );
end component;

----------------------------------------
--
-- PS/2 Keyboard
--
----------------------------------------

component keyboard
  generic(
    KBD_CLK_FREQ : integer := CPU_CLK_FREQ
  );
  port(
    clk             : in    std_logic;
    rst             : in    std_logic;
    cs              : in    std_logic;
    addr            : in    std_logic;
    rw              : in    std_logic;
    data_in         : in    std_logic_vector(7 downto 0);
    data_out        : out   std_logic_vector(7 downto 0);
    irq             : out   std_logic;
    kbd_clk         : inout std_logic;
    kbd_data        : inout std_logic
  );
end component;

----------------------------------------
--
-- Video Display Unit.
--
----------------------------------------
component vdu8
      generic(
        VGA_CLK_FREQ           : integer := VGA_CLK_FREQ; -- 25MHz
	     VGA_HOR_CHARS          : integer := 80; -- CHARACTERS 25.6us
	     VGA_HOR_CHAR_PIXELS    : integer :=  8; -- PIXELS 0.32us
	     VGA_HOR_FRONT_PORCH    : integer := 16; -- PIXELS 0.64us (0.94us)
	     VGA_HOR_SYNC           : integer := 96; -- PIXELS 3.84us (3.77us)
	     VGA_HOR_BACK_PORCH     : integer := 48; -- PIXELS 1.92us (1.89us)
	     VGA_VER_CHARS          : integer := 25; -- CHARACTERS 12.8ms
	     VGA_VER_CHAR_LINES     : integer := 16; -- LINES 0.512ms
	     VGA_VER_FRONT_PORCH    : integer := 10; -- LINES 0.320ms
	     VGA_VER_SYNC           : integer :=  2; -- LINES 0.064ms
	     VGA_VER_BACK_PORCH     : integer := 34  -- LINES 1.088ms
      );
      port(
		-- control register interface
      vdu_clk      : in  std_logic;	 -- CPU Clock - 12.5MHz
      vdu_rst      : in  std_logic;
		vdu_cs       : in  std_logic;
		vdu_rw       : in  std_logic;
		vdu_addr     : in  std_logic_vector(2 downto 0);
      vdu_data_in  : in  std_logic_vector(7 downto 0);
      vdu_data_out : out std_logic_vector(7 downto 0);

      -- vga port connections
		vga_clk      : in  std_logic;	-- VGA Pixel Clock - 25 MHz
      vga_red_o    : out std_logic;
      vga_green_o  : out std_logic;
      vga_blue_o   : out std_logic;
      vga_hsync_o  : out std_logic;
      vga_vsync_o  : out std_logic
   );
end component;

----------------------------------------
--
-- Sound
--
----------------------------------------

component sound is
	port (	
	clk        : in  std_logic;
	clk_50     : in  std_logic;
    rst        : in  std_logic;
    cs         : in  std_logic;
    addr       : in  std_logic_vector(5 downto 0);
    rw         : in  std_logic;
    data_in    : in  std_logic_vector(7 downto 0);
	data_out   : out std_logic_vector(7 downto 0);
	audio_out  : out std_logic
  );
end component;


begin

-----------------------------------------------------------------------------
-- Instantiation of internal components
-----------------------------------------------------------------------------

----------------------------------------
--
-- Clock generator
--
----------------------------------------

my_clock_div: clock_div port map (
    clk_in   => CLK_50MHZ,  -- Clock input
    sys_clk  => sys_clk,  -- System Clock Out        (1/1)
    vga_clk  => vga_clk,  -- CPU/VGA Pixel Clock Out (1/2)
    cpu_clk  => open      --                         (1/4)
  );

-----------------------------------------
--
-- LED Flasher
--
-----------------------------------------

my_LED_flasher : flasher port map (
    clk      => cpu_clk,
    rst      => cpu_rst,
    LED      => LED(0)
  );

----------------------------------------
--
-- 6809 compatible CPU
--
----------------------------------------

my_cpu : cpu09  port map (    
	 clk	     => cpu_clk,
    rst       => cpu_rst,
    vma       => cpu_vma,
    addr      => cpu_addr(15 downto 0),
    rw	     => cpu_rw,
	 data_out  => cpu_data_out,
    data_in   => cpu_data_in,
	 irq       => cpu_irq,
	 nmi       => cpu_nmi,
	 firq      => cpu_firq,
	 halt      => cpu_halt,
	 hold      => cpu_hold
  );

my_rom : mon_rom port map (
       clk      => cpu_clk,
		 rst      => cpu_rst,
		 cs       => rom_cs,
       addr     => cpu_addr(10 downto 0),
		 rw       => '1',
       data_in  => cpu_data_out,
       data_out => rom_data_out
    );

my_ram : ram_32k port map (
       clk      => cpu_clk,
		 rst      => cpu_rst,
		 cs       => ram_cs,
       addr     => cpu_addr(14 downto 0),
		 rw       => cpu_rw,
       data_in  => cpu_data_out,
       data_out => ram_data_out
    );

my_pia  : pia_timer port map (
	 clk	     => cpu_clk,
	 rst       => cpu_rst,
    cs        => pia_cs,
    addr      => cpu_addr(1 downto 0),
	 rw        => cpu_rw,
	 data_in   => cpu_data_out,
	 data_out  => pia_data_out,
    irqa      => pia_irq_a,
    irqb      => pia_irq_b
	 );

----------------------------------------
--
-- ACIA/UART Serial interface
--
----------------------------------------
my_ACIA  : acia6850 port map (
	 clk	     => cpu_clk,
	 rst       => cpu_rst,
    cs        => uart_cs,
    addr      => cpu_addr(0),
	 rw        => cpu_rw,
	 data_in   => cpu_data_out,
	 data_out  => uart_data_out,
    irq       => uart_irq,
	 RxC       => uart_clk,
	 TxC       => uart_clk,
	 RxD       => RS232_DCE_RXD,
	 TxD       => RS232_DCE_TXD,
	 DCD_n     => '0',
	 CTS_n     => '0',
	 RTS_n     => open
	 );

----------------------------------------
--
-- MIDI ACIA/UART Serial interface
--
----------------------------------------
my_MIDI_ACIA  : acia6850 port map (
	 clk	     => cpu_clk,
	 rst       => cpu_rst,
    cs        => midi_uart_cs,
    addr      => cpu_addr(0),
	 rw        => cpu_rw,
	 data_in   => cpu_data_out,
	 data_out  => midi_uart_data_out,
    irq       => midi_uart_irq,
	 RxC       => midi_uart_clk,
	 TxC       => midi_uart_clk,
	 RxD       => MIDI_RS232_DCE_RXD,
	 TxD       => MIDI_RS232_DCE_TXD,
	 DCD_n     => '0',
	 CTS_n     => '0',
	 RTS_n     => open
	 );


----------------------------------------
--
-- ACIA Clock
--
----------------------------------------
my_ACIA_Clock : ACIA_Clock
  generic map(
    SYS_CLK_FREQ  => SYS_CLK_FREQ,
	 ACIA_CLK_FREQ => ACIA_CLK_FREQ
  ) 
  port map(
    clk        => sys_clk,
    acia_clk   => uart_clk
  ); 

----------------------------------------
--
-- MIDI ACIA Clock
--
----------------------------------------
my_MIDI_ACIA_Clock : ACIA_Clock
  generic map(
    SYS_CLK_FREQ  => SYS_CLK_FREQ,
	 ACIA_CLK_FREQ => MIDI_ACIA_CLK_FREQ
  ) 
  port map(
    clk        => sys_clk,
    acia_clk   => midi_uart_clk
  ); 


----------------------------------------
--
-- PS/2 Keyboard Interface
--
----------------------------------------
my_keyboard : keyboard
   generic map (
     KBD_CLK_FREQ => CPU_CLK_FREQ
	) 
   port map(
	  clk          => cpu_clk,
	  rst          => cpu_rst,
	  cs           => kbd_cs,
	  addr         => cpu_addr(0),
	  rw           => cpu_rw,
	  data_in      => cpu_data_out(7 downto 0),
	  data_out     => kbd_data_out(7 downto 0),
	  irq          => kbd_irq,
	  kbd_clk      => PS2_CLK,
	  kbd_data     => PS2_DATA
	);

----------------------------------------
--
-- Video Display Unit instantiation
--
----------------------------------------
my_vdu : vdu8 
  generic map(
      VGA_CLK_FREQ           => VGA_CLK_FREQ, -- HZ
	   VGA_HOR_CHARS          => 80, -- CHARACTERS
	   VGA_HOR_CHAR_PIXELS    =>  8,  -- PIXELS
	   VGA_HOR_FRONT_PORCH    => 16, -- PIXELS
	   VGA_HOR_SYNC           => 96, -- PIXELS
	   VGA_HOR_BACK_PORCH     => 48, -- PIXELS
	   VGA_VER_CHARS          => 25, -- CHARACTERS
	   VGA_VER_CHAR_LINES     => 16, -- LINES
	   VGA_VER_FRONT_PORCH    => 10, -- LINES
	   VGA_VER_SYNC           =>  2, -- LINES
	   VGA_VER_BACK_PORCH     => 34  -- LINES
  )
  port map(

		-- Control Registers
		vdu_clk       => cpu_clk,					 -- 25 MHz System Clock in
      vdu_rst       => cpu_rst,
		vdu_cs        => vdu_cs,
		vdu_addr      => cpu_addr(2 downto 0),
		vdu_rw        => cpu_rw,
		vdu_data_in   => cpu_data_out,
		vdu_data_out  => vdu_data_out,

      -- vga port connections
      vga_clk       => vga_clk,					 -- 25 MHz VDU pixel clock
      vga_red_o     => vga_red,
      vga_green_o   => vga_green,
      vga_blue_o    => vga_blue,
      vga_hsync_o   => vga_hsync,
      vga_vsync_o   => vga_vsync
   );


----------------------------------------
--
-- Timer Module
--
----------------------------------------
my_timer  : timer port map (
    clk       => cpu_clk,
	 rst       => cpu_rst,
    cs        => timer_cs,
	 rw        => cpu_rw,
    addr      => cpu_addr(0),
	 data_in   => cpu_data_out,
	 data_out  => timer_data_out,
    irq       => timer_irq
    );

----------------------------------------
--
-- Bus Trap Interrupt logic
--
----------------------------------------
my_trap : trap port map (	
	 clk        => cpu_clk,
    rst        => cpu_rst,
    cs         => trap_cs,
    rw         => cpu_rw,
	 vma        => cpu_vma,
    addr       => cpu_addr,
    data_in    => cpu_data_out,
	 data_out   => trap_data_out,
	 irq        => trap_irq
    );

----------------------------------------
--
-- Sound
--
----------------------------------------

my_sound : sound port map (
    clk       => cpu_clk,
    clk_50    => sys_clk,
    rst     => cpu_rst,
    cs      => sound_cs,
    addr    => cpu_addr(5 downto 0),
    rw        => cpu_rw,
    data_in    => cpu_data_out,
    data_out   => sound_data_out,
    audio_out    => audio_left 
);

vga_colors: process(vga_red, vga_green, vga_blue)
begin
    vga_r <= vga_red & vga_red & vga_red & vga_red;
    vga_g <= vga_green & vga_green & vga_green & vga_green;
    vga_b <= vga_blue & vga_blue & vga_blue & vga_blue;
end process;

----------------------------------------------------------------------
--
-- Process to decode memory map
--
----------------------------------------------------------------------

mem_decode: process( cpu_addr, cpu_rw, cpu_vma,
					      rom_data_out, 
							ram_data_out,
						   timer_data_out, 
							trap_data_out, 
							pia_data_out,
						   uart_data_out,
							kbd_data_out,
							vdu_data_out )
begin

	rom_cs   <= '0';
	ram_cs   <= '0';
	uart_cs  <= '0';
	midi_uart_cs <= '0';
	timer_cs <= '0';
	trap_cs  <= '0';
	pia_cs   <= '0';
	kbd_cs   <= '0';
	vdu_cs   <= '0';
	sound_cs  <= '0';

      case cpu_addr(15 downto 12) is
	   --
		-- SBUG/KBUG/SYS09BUG Monitor ROM $F800 - $FFFF
		--
		when "1111" => -- $F000 - $FFFF
		   cpu_data_in <= rom_data_out;
			rom_cs      <= cpu_vma;              -- read ROM

      --
		-- IO Devices $E000 - $EFFF
		--
		when "1110" => -- $E000 - $E7FF
		   case cpu_addr(7 downto 4) is
			--
			-- UART / ACIA $E000
			--
			when "0000" => -- $E000
		     cpu_data_in <= uart_data_out;
			  uart_cs     <= cpu_vma;

			--
			-- MIDI UART / ACIA $E010-$E01F
			--
			when "0001" => -- $E010
           cpu_data_in <= midi_uart_data_out;
           midi_uart_cs <= cpu_vma;

         --
         -- Keyboard port $E020 - $E02F
			--
			when "0010" => -- $E020
           cpu_data_in <= kbd_data_out;
			  kbd_cs <= cpu_vma;

         --
         -- VDU port $E030 - $E03F
			--
			when "0011" => -- $E030
           cpu_data_in <= vdu_data_out;
			  vdu_cs      <= cpu_vma;

         --
			-- Compact Flash $E040 - $E04F
			--
			when "0100" => -- $E040
           cpu_data_in <= (others => '0');

         --
         -- Timer $E050 - $E05F
			--
			when "0101" => -- $E050
           cpu_data_in <= timer_data_out;
           timer_cs    <= cpu_vma;

         --
         -- Bus Trap Logic $E060 - $E06F
			--
			when "0110" => -- $E060
           cpu_data_in <= trap_data_out;
			  trap_cs     <= cpu_vma;

         --
         -- PIA Timer $E070 - $E07F
			--
			when "0111" => -- $E070
           cpu_data_in <= pia_data_out;
			  pia_cs      <= cpu_vma;
			  
            --
            -- Sound $E080 - $E09F 
			--
			  
			when "1000" => -- $E080
			    -- voice 0
    			cpu_data_in <= sound_data_out;
                sound_cs    <= cpu_vma;
                
			when "1001" => -- $E090
			    -- voice 1
			    cpu_data_in <= sound_data_out;
			    sound_cs    <= cpu_vma;

            when "1010" => -- $E0A0
                -- voice 2
                cpu_data_in <= sound_data_out;
                sound_cs <= cpu_vma;
            
            when "1011" => -- $E0B0
                -- voice 3
                cpu_data_in <= sound_data_out;
                sound_cs <= cpu_vma;
			  
			when others => -- $E0D0 to $E7FF
           cpu_data_in <= (others => '0');
		   end case;

		--
		-- $8000 to $DFFF = null
		--
      when "1101" | "1100" | "1011" | "1010" |
		     "1001" | "1000" =>
		  cpu_data_in <= (others => '0');

		--
		-- Everything else is RAM
		--
		when others =>
		  cpu_data_in <= ram_data_out;
		  ram_cs      <= cpu_vma;
		end case;
end process;

--
-- Assign CPU clock, reset, interrupt, halt & hold signals
-- as well as LED signals
--
assign_signals : process( vga_clk, BTN_SOUTH, 
                      pia_irq_a, pia_irq_b, uart_irq, midi_uart_irq, trap_irq, timer_irq, kbd_irq
							 )
begin
    cpu_clk  <= vga_clk;
 	 cpu_rst  <= BTN_SOUTH; -- CPU reset is active high
    cpu_irq  <= uart_irq or midi_uart_irq or kbd_irq;
	 cpu_nmi  <= pia_irq_a or trap_irq;
	 cpu_firq <= pia_irq_b or timer_irq;
	 cpu_halt <= '0';
    cpu_hold <= '0';

    -- LED outputs
    LED(7 downto 1) <= (others=>'1');

end process;

process(audio_left, audio_right)
begin
    aud_l <= audio_left;
    aud_r <= audio_left;
   	aux_aud_l <= audio_left;
	aux_aud_r <= audio_left;
end process;

end my_computer; --===================== End of architecture =======================--

