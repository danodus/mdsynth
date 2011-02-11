library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.std_logic_unsigned.all; 


entity dac is
	port (	clk:		in std_logic;
				dac_in:	in std_logic_vector(7 downto 0);
				reset:	in std_logic;
				dac_out:	out std_logic);
end entity dac;

architecture dac_arch of dac is
signal delta_adder : std_logic_vector(9 downto 0);
signal sigma_adder : std_logic_vector(9 downto 0);
signal sigma_latch : std_logic_vector(9 downto 0);
signal delta_b : std_logic_vector(9 downto 0);
begin
	delta_b(9 downto 8) <= sigma_latch(9) & sigma_latch(9);
	delta_b(7 downto 0) <= (others => '0');
	delta_adder <= dac_in + delta_b;
	sigma_adder <= delta_adder + sigma_latch;
	process (clk, reset)
	begin
		if (reset = '1') then
			sigma_latch <= ('1', others => '0');
			dac_out <= '0';
		elsif (clk'event and clk = '1') then
			sigma_latch <= sigma_adder;
			dac_out <= sigma_latch(9);
		end if;
	end process;
end architecture dac_arch;



