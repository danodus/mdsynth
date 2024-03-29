-- MDSynth Sound Chip
--
-- Copyright (c) 2012-2022, Daniel Cliche
-- All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
-- following conditions are met:
--
--    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
--      following disclaimer.
--    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
--      following disclaimer in the documentation and/or other materials provided with the distribution.
--
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
-- INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
-- DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
-- SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
-- SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
-- WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
-- USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity channel is
    port ( clk:      in std_logic;
           reset:    in std_logic;
           waveform: in std_logic_vector(2 downto 0);    -- 0: DAC direct, 1: Square (message only), 2: Sawtooth (message only), 3: Sine (message only), 4: FM (implemented as phase modulation), 5: Noise
           note_on:  in std_logic;
           gain_message:          in unsigned(5 downto 0);
           gain_modulated:        in unsigned(5 downto 0);
           phase_delta:           in unsigned(11 downto 0);
           octave_message:        in unsigned(3 downto 0);
           octave_carrier:        in unsigned(3 downto 0);
           waveform_message:      in std_logic_vector(1 downto 0);  -- 0: Full sine (++--), 1: Half sine (++00), 3: Full sine positive (++++), 4: Quarter sine positive (+0+0)
           waveform_modulated:    in std_logic_vector(1 downto 0);  -- 0: Full sine (++--), 1: Half sine (++00), 3: Full sine positive (++++), 4: Quarter sine positive (+0+0)
           attack_rate:           in unsigned(3 downto 0);
           release_rate:          in unsigned(3 downto 0);           
           reset_phase:           in std_logic;
           dac_direct_value:      in std_logic_vector(7 downto 0);
           output:                out std_logic_vector(7 downto 0);
           envelope_phase:        out std_logic_vector(1 downto 0));
end channel;

architecture channel_arch of channel is

component sinewave is
	port (    clk:          in std_logic;
              gain:         in unsigned(5 downto 0);
	          phase:		in  unsigned(7 downto 0);
              waveform:     in std_logic_vector(1 downto 0);  -- 0: Full sine (++--), 1: Half sine (++00), 3: Full sine positive (++++), 4: Quarter sine positive (+0+0)
			  data_out: 	out integer range -128 to 127);
end component;

component nco is
    port ( clk:         in std_logic;
           reset_phase: in std_logic;
           ena:         in std_logic;
           phase_delta: in unsigned(11 downto 0);
           octave:      in unsigned(3 downto 0);
           phase:       out unsigned(7 downto 0));
end component;

component envelope is
port (  clk:		in std_logic;
        reset:	    in std_logic;
        note_on:    in std_logic;
        attack_rate:    in unsigned(3 downto 0);
        release_rate:   in unsigned(3 downto 0);        
        gain:       out unsigned(5 downto 0);
        phase:      out std_logic_vector(1 downto 0));
end component;

signal dac_in : std_logic_vector(7 downto 0);

signal phase_carrier : unsigned(7 downto 0);
signal phase_message : unsigned(7 downto 0);
signal phase_modulated  : unsigned(8 downto 0);

signal sine_message : integer range -128 to 127 := 0;
signal sine_modulated : integer range -128 to 127 := 0;

type state_type is (
    sinewave_message_state,
    sinewave_modulated_state);

signal state : state_type := sinewave_message_state;

signal sine_gain : unsigned(5 downto 0);
signal sine_phase : unsigned(7 downto 0);
signal sine_waveform : std_logic_vector(1 downto 0);
signal sine_data : integer range -128 to 127;

signal ena_nco_carrier : std_logic;
signal ena_nco_message : std_logic;

signal envelope_reset : std_logic := '1';
signal envelope_gain : unsigned(5 downto 0);

signal lfsr_counter : unsigned(11 downto 0);
signal lfsr_data : std_logic_vector(31 downto 0);

begin

    nco_carrier0 : nco port map (clk => clk, reset_phase => reset_phase, ena => ena_nco_carrier, phase_delta => phase_delta, octave => octave_carrier, phase => phase_carrier);
    nco_message0 : nco port map (clk => clk, reset_phase => reset_phase, ena => ena_nco_message, phase_delta => phase_delta, octave => octave_message, phase => phase_message);

    sinewave0 : sinewave port map (clk => clk, gain => sine_gain, phase => sine_phase, waveform => sine_waveform, data_out => sine_data);

    envelope0 : envelope port map (clk => clk, reset => envelope_reset, note_on => note_on, attack_rate => attack_rate, release_rate => release_rate, gain => envelope_gain, phase => envelope_phase);
    
	lfsr0 : entity work.lfsr
	    generic map (
	    g_Num_Bits => 32
	    )
        port map (
            i_Clk   => std_logic(lfsr_counter(11)),
            i_Enable    => '1',
            i_Seed_DV   => '0',
            i_Seed_Data => (31 downto 0 => '0'),
            o_LFSR_Data => lfsr_data            
        );
          
	process (clk, reset)
	begin
        if (reset = '1') then
            dac_in <= "00000000";
            sine_gain <= to_unsigned(0, 6);
            sine_phase <= to_unsigned(0, 8);
            sine_waveform <= "00";
            envelope_reset <= '1';
	    elsif (rising_edge(clk)) then
	        lfsr_counter <= lfsr_counter + 1;
            envelope_reset <= '0';	    
            phase_modulated <= to_unsigned(to_integer(phase_carrier) + sine_message, 9);
            output <= dac_in;
            case state is
	            when sinewave_message_state =>
	                -- We calculate the message waveform
	                ena_nco_message <= '1';
	                ena_nco_carrier <= '0';
	                sine_phase <= phase_message;
	                sine_gain <= gain_message;
	                sine_waveform <= waveform_message;
	                sine_message <= sine_data;
	                state <= sinewave_modulated_state;
	            when sinewave_modulated_state =>
	                -- We calculate the modulated waveform
	                ena_nco_message <= '0';
	                ena_nco_carrier <= '1';
	                if waveform = "101" then
	                   -- noise
	                    sine_phase <= unsigned(lfsr_data(7 downto 0));
	                else
                        sine_phase <= phase_modulated(7 downto 0);
                    end if;
	                --sine_gain <= gain_modulated;
	                sine_gain <= envelope_gain;
	                sine_waveform <= waveform_modulated;
	                sine_modulated <= sine_data;
	                state <= sinewave_message_state;
	        end case;
	        
	        -- We update the DAC
	        
	        case waveform is
	            when "001" =>
	                -- Square wave
	                if (phase_message(7) = '1') then
	                    dac_in(7 downto 0) <= (others => '1');
	                else
	                    dac_in(7 downto 0) <= (others => '0');
	                end if;
	            when "010" =>
	                -- Sawtooth wave
	                dac_in <= std_logic_vector(phase_message);
	            when "011" =>
	                -- Sine wave
	                dac_in <= std_logic_vector(to_unsigned(128 + sine_message, 8));
	            when "100" =>
	                -- PM
	                dac_in <= std_logic_vector(to_unsigned(128 + sine_modulated, 8));
	            when "101" =>
                    -- Noise
	                dac_in <= std_logic_vector(to_unsigned(128 + sine_modulated, 8));
	            when others =>
	                -- DAC direct
	                dac_in <= dac_direct_value;
	        end case;	                
	        

		end if;
		
	end process;
	
end channel_arch;

