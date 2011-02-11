library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity sinewave is
	port (clk 		: in  std_logic;
			data_out : out integer range -128 to 127);
end sinewave;

architecture Behavioral of sinewave is
signal i : integer range 0 to 30:=0;
type memory_type is array (0 to 29) of integer range -128 to 127;
signal sine : memory_type :=(0,16,31,45,58,67,74,77,77,74,67,58,45,31,16,0,-16,-31,-45,-58,-67,-74,-77,-77,-74,-67,-58,-45,-31,-16);

begin

	process(clk)
		begin
		if(rising_edge(clk)) then    
			data_out <= sine(i);
			i <= i+ 1;
			if(i = 29) then
				i <= 0;
			end if;
		end if;
	end process;

end Behavioral;