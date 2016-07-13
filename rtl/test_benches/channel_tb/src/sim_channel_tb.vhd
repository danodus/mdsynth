library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use STD.TEXTIO.ALL;
use IEEE.STD_LOGIC_TEXTIO.ALL;

entity sim_channel_tb is
generic (
           log_file:       string  := "res.log"
          );
end sim_channel_tb;

architecture sim_channel_tb_arch of sim_channel_tb is

    component channel_tb is
        port ( clk_50mhz:   in std_logic; 
 		  btn_south:   in std_logic;
 		  btn_north:   in std_logic;
          sw:          in std_logic_vector(3 downto 0);
          aud_l:       out std_logic;
          aud_r:       out std_logic;
          aux_aud_l:   out std_logic;
          aux_aud_r:   out std_logic;
          dac_output:  out std_logic_vector(7 downto 0)
          );
    end component;
    
    signal clk : std_logic := '0';
    constant clk_half_period : time := 10 ns;
    
    signal btn_south : std_logic := '1';
    signal btn_north : std_logic := '0';
    signal sw : std_logic_vector(3 downto 0) := "0100";
    signal aud_l : std_logic;
    signal aud_r : std_logic;
    signal aux_aud_l : std_logic;
    signal aux_aud_r : std_logic;
    signal dac_output : std_logic_vector(7 downto 0);
    
    file l_file: TEXT open write_mode is log_file;

begin
    
    channel_tb0: channel_tb port map (
        clk_50mhz => clk,
        btn_south => btn_south,
        btn_north => btn_north,
        sw => sw,
        aud_l => aud_l,
        aud_r => aud_r,
        aux_aud_l => aux_aud_l,
        aux_aud_r => aux_aud_r,
        dac_output => dac_output
        );
        
     clk <= not clk after clk_half_period;
     
     my_process : process
     begin
     
        wait for 1us;
        btn_south <= '0';
        wait for 2us;
        btn_north <= '1';
        while true loop
            wait until clk = '1';
        end loop;
     end process;
     
     -- write data and control information to a file
     write_data: process
        variable my_line : line;
        variable my_counter : integer := 0;
     begin                                       
     
        wait until btn_north='1';
        
        while true loop
            if (my_counter mod (50000000 / 44100) = 0) then
                write(my_line, to_integer(unsigned(dac_output)));
                writeline(l_file, my_line);
            end if;
            my_counter := my_counter + 1;
            wait until clk = '1';
        end loop;
     
     end process;

end sim_channel_tb_arch;