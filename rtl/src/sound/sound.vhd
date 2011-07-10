library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity sound is
	port (	
	clk        : in  std_logic;
	clk_50     : in  std_logic;
    rst        : in  std_logic;
    cs         : in  std_logic;
    addr       : in  std_logic_vector(1 downto 0);
    rw         : in  std_logic;
    data_in    : in  std_logic_vector(7 downto 0);
	data_out   : out std_logic_vector(7 downto 0);
	audio_out  : out std_logic
  );
end;

architecture sound_arch of sound is

component channel is
    port ( clk:      in std_logic;
           reset:    in std_logic;
           waveform: in std_logic_vector(2 downto 0);    -- 0: None, 1: Square, 2: Sawtooth, 3: Sine, 4: FM
           pitch_message:    in unsigned(6 downto 0);            -- 60 = C4
           pitch_carrier:    in unsigned(6 downto 0);            -- 60 = C4
           output:   out std_logic);
end component;

signal waveform : std_logic_vector(2 downto 0);
signal pitch_message : unsigned(6 downto 0);
signal pitch_carrier : unsigned(6 downto 0);

begin

    channel0 : channel port map (clk => clk_50, reset => rst, waveform => waveform, pitch_message => pitch_message, pitch_carrier => pitch_carrier, output => audio_out);

    process(clk)
    begin
        if clk'event and clk = '0' then
            if rst = '1' then
                pitch_message <= to_unsigned(0, 7);
                pitch_carrier <= to_unsigned(0, 7);
                waveform <= (others=>'0');
            elsif cs = '1' and rw = '0' then
                case addr is
                    when "00" => waveform <= data_in(2 downto 0);
                    when "01" => pitch_message <= unsigned(data_in(6 downto 0));
                    when others => pitch_carrier <= unsigned(data_in(6 downto 0));
                end case;
            end if;
        end if;
    end process;

end sound_arch;
	
