-- MDSynth Sound Chip Test Bench
--
-- Copyright (c) 2012-2016, Meldora Inc.
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

entity channel_tb is
   port ( clk_50mhz:   in std_logic; 
 		  btn_south:   in std_logic;
 		  btn_north:   in std_logic;
          sw:          in std_logic_vector(3 downto 0);
          aud_l:       out std_logic;
          aud_r:       out std_logic;
          aux_aud_l:   out std_logic;
          aux_aud_r:   out std_logic;
          dac_output: out std_logic_vector(7 downto 0)
          );
 
end entity channel_tb;

architecture channel_tb_arch of channel_tb is

component channel is
    port ( clk:      in std_logic;
           reset:    in std_logic;
           waveform: in std_logic_vector(2 downto 0);    -- 0: DAC direct, 1: Square (message only), 2: Sawtooth (message only), 3: Sine (message only), 4: FM (implemented as phase modulation), 5: DAC-direct
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
           output:                out std_logic;
           dac_output:            out std_logic_vector(7 downto 0));
end component;

component pitch_to_freq is
    port ( pitch:         in unsigned(6 downto 0);      -- 60 = C4
           phase_delta:   out unsigned(11 downto 0);
           octave:        out unsigned(3 downto 0));
end component;

signal channel_out: std_logic;
signal pitch_message: unsigned (6 downto 0) := to_unsigned(69, 7);
signal pitch_carrier: unsigned (6 downto 0) := to_unsigned(69, 7);
signal counter: unsigned (31 downto 0) := to_unsigned(0, 32);
signal waveform: std_logic_vector(2 downto 0) := "100";

signal phase_delta_message: unsigned(11 downto 0);
signal phase_delta_carrier: unsigned(11 downto 0);
signal octave_message: unsigned(3 downto 0);
signal octave_carrier: unsigned(3 downto 0);

signal gain_message: unsigned(5 downto 0) := to_unsigned(63, 6);
signal gain_modulated: unsigned(5 downto 0) := to_unsigned(63, 6);

signal reset_phase: std_logic := '0';
signal dac_direct_value: std_logic_vector(7 downto 0) := "00000000";

signal note_on: std_logic := '0';

signal waveform_message: std_logic_vector(1 downto 0) := "00";
signal waveform_modulated: std_logic_vector(1 downto 0) := "00";

signal attack_rate: unsigned(3 downto 0) := to_unsigned(15, 4);
signal release_rate: unsigned(3 downto 0) := to_unsigned(15, 4);

begin

    pitch_to_freq_carrier0: pitch_to_freq port map (pitch => pitch_message, phase_delta => phase_delta_message, octave => octave_message);
    pitch_to_freq_message0: pitch_to_freq port map (pitch => pitch_carrier, phase_delta => phase_delta_carrier, octave => octave_carrier);
    channel0: channel port map (
        clk => clk_50mhz,
        reset => btn_south,
        waveform => waveform,
        note_on => note_on,
        gain_message => gain_message,
        gain_modulated => gain_modulated,
        phase_delta => phase_delta_message,     -- Note: phase_delta_carrier is not used
        octave_message => octave_message,
        octave_carrier => octave_carrier,
        waveform_message => waveform_message,
        waveform_modulated => waveform_modulated,
        attack_rate => attack_rate,
        release_rate => release_rate,
        reset_phase => reset_phase,
        dac_direct_value => dac_direct_value,
        output => channel_out,
        dac_output => dac_output);

    waveform <= sw(2 downto 0);

    aud_l <= channel_out;
	aud_r <= channel_out;
	aux_aud_l <= channel_out;
	aux_aud_r <= channel_out;
	
	process (clk_50mhz, btn_south)
	begin
	    if (btn_south = '1') then
	       reset_phase <= '1';
	    elsif (rising_edge(clk_50mhz)) then
	       reset_phase <= '0';
	       note_on <= btn_north;
	       
--	        if (counter = 0) then
	            --counter <= to_unsigned(5000000, 32);
--	            counter <= to_unsigned(200, 32);
--	            note_on <= not note_on;
--	            pitch_message <= pitch_message + 1;
--	            if (pitch_message = 0) then
--        	        gain_message <= gain_message - 8;
--	                gain_modulated <= gain_modulated - 8;	        
--	            end if;
--	        else
--	            counter <= counter - 1;
--	        end if;
	    end if;
	end process;
	
end architecture channel_tb_arch;
