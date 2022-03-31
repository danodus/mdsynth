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
--
-- Write:
--
-- Base + 0: bits 2 downto 0: Waveform (0x00: DAC direct, 0x01: Square, 0x02: Sawtooth, 0x03: Sine, 0x04: Phase Modulation, 0x05: Noise)
--           bit 7: Note ON/OFF (0: OFF, 1: ON)
-- Base + 1: Pitch Hi (see below)
-- Base + 2: Pitch Low (see below)
-- Base + 3: Carrier Octave (4-bit, PM only)
-- Base + 4: bits 5 downto 0: Message Gain (6-bit, sine and PM only)
--           bits 7 downto 6: Waveform Message (0: Full sine (++--), 1: Half sine (++00), 3: Full sine positive (++++), 4: Quarter sine positive (+0+0))
-- Base + 5: bits 5 downto 0: Modulated Gain (6-bit, PM only)
--           bits 7 downto 6: Waveform Modulated (0: Full sine (++--), 1: Half sine (++00), 3: Full sine positive (++++), 4: Quarter sine positive (+0+0))
-- Base + 6: DAC value (8-bit, DAC-direct only)
-- Base + 7: bits 3 downto 0: Attack Rate (0xF: fastest)
--           bits 7 downto 4: Release Rate (0xF: fastest)
--
-- Read:

-- Base + 0: bits 1 downto 0: Envelope Phase (0x0: Idle, 0x1: Attack, 0x2: Sustain, 0x3: Release)

--
-- The pitch is a 16-bit value with the following format: 
--     bits 15 downto 12: octave    (A4 is octave 5)
--     bits 11 downto 0: phase_delta (see below)
--
--     freq = (50E6 * (phase_delta * 2^octave)) / 2^32
--     phase_delta = freq * 2^32 / (50E6 * 2^octave)

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity sound is
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
end;

architecture sound_arch of sound is

constant nb_voices : integer := 4;

component channel is
    port ( clk:      in std_logic;
           reset:    in std_logic;
           waveform: in std_logic_vector(2 downto 0);    -- 0: DAC direct, 1: Square (message only), 2: Sawtooth (message only), 3: Sine (message only), 4: Phase Modulation, 5: Noise
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
end component;

component dac is
	port (  clk:        in std_logic;
            dac_in:     in std_logic_vector(9 downto 0);
            reset:      in std_logic;
            dac_out:    out std_logic);
end component;

type voices_waveform_type is array (0 to nb_voices) of std_logic_vector(2 downto 0);
signal voices_waveform : voices_waveform_type;

type voices_pitch_type is array (0 to nb_voices) of unsigned(15 downto 0);
signal voices_pitch: voices_pitch_type;

type voices_phase_delta_type is array (0 to nb_voices) of unsigned(11 downto 0);
signal voices_phase_delta: voices_phase_delta_type;

type voices_octave_type is array (0 to nb_voices) of unsigned(3 downto 0);
signal voices_octave_message: voices_octave_type;
signal voices_octave_carrier: voices_octave_type;

type voices_gain_type is array (0 to nb_voices) of unsigned(5 downto 0);
signal voices_gain_message: voices_gain_type := (others => to_unsigned(63, 6));
signal voices_gain_modulated: voices_gain_type  := (others => to_unsigned(63, 6));

type voices_op_waveform_type is array (0 to nb_voices) of std_logic_vector(1 downto 0);
signal voices_waveform_message: voices_op_waveform_type := (others => "00");
signal voices_waveform_modulated: voices_op_waveform_type := (others => "00");

signal voices_reset_phase: std_logic_vector(0 to nb_voices) := (others => '0');

type voices_note_on_type is array (0 to nb_voices) of std_logic;
signal voices_note_on: voices_note_on_type := (others => '0');

type voices_dac_direct_value_type is array (0 to nb_voices) of std_logic_vector(7 downto 0);
signal voices_dac_direct_value: voices_dac_direct_value_type := (others => (others => '0'));

type voices_rate_type is array (0 to nb_voices) of unsigned(3 downto 0);
signal voices_attack_rate: voices_rate_type := (others => to_unsigned(0, 4));
signal voices_release_rate: voices_rate_type := (others => to_unsigned(0, 4));

signal dac_in: std_logic_vector(9 downto 0);

type voices_output_type is array (0 to nb_voices) of std_logic_vector(9 downto 0);
signal voices_output: voices_output_type := (others => (others =>'0'));

type voices_envelope_phase_type is array (0 to nb_voices) of std_logic_vector(1 downto 0);
signal voices_envelope_phase: voices_envelope_phase_type := (others => (others =>'0'));

begin

    dac0 : dac port map (clk => clk_50, dac_in => dac_in, reset => rst, dac_out => audio_out);

    channels : for i in 0 to 3 generate
        channelx : channel port map(
            clk => clk_50,
            reset => rst,
            waveform => voices_waveform(i),
            note_on => voices_note_on(i),
            gain_message => voices_gain_message(i),
            gain_modulated => voices_gain_modulated(i),
            phase_delta => voices_phase_delta(i),
            octave_message => voices_octave_message(i),
            octave_carrier => voices_octave_carrier(i),
            waveform_message => voices_waveform_message(i),
            waveform_modulated => voices_waveform_modulated(i),
            attack_rate => voices_attack_rate(i),
            release_rate => voices_release_rate(i),
            reset_phase => voices_reset_phase(i),
            dac_direct_value => voices_dac_direct_value(i),
            output => voices_output(i)(7 downto 0),
            envelope_phase => voices_envelope_phase(i));
             
    end generate channels;

    process(clk_50)
    begin
        if clk_50'event and clk_50 = '1' then
            dac_in <= std_logic_vector(unsigned(voices_output(0)) + unsigned(voices_output(1)) + unsigned(voices_output(2)) + unsigned(voices_output(3)));
        end if;
    end process;

    process(clk)
    begin
        if clk'event and clk = '0' then
            if rst = '1' then
                voices_pitch <= (others => to_unsigned(0, 16));
                voices_waveform <= (others => (others=>'0'));
                voices_reset_phase <= (others=>'1');
                voices_note_on <= (others=>'0');
            elsif cs = '1' then
                if rw = '0' then
                    -- write data
                    case addr(3 downto 0) is
                        when "0000" => voices_waveform(to_integer(unsigned(addr(5 downto 4)))) <= data_in(2 downto 0);
                                      voices_note_on(to_integer(unsigned(addr(5 downto 4)))) <= data_in(7);
                                      if (data_in(7) = '1') then
                                        voices_reset_phase(to_integer(unsigned(addr(5 downto 4)))) <= '1';
                                      end if;
                        when "0001" => voices_pitch(to_integer(unsigned(addr(5 downto 4))))(15 downto 8) <= unsigned(data_in);
                        when "0010" => voices_pitch(to_integer(unsigned(addr(5 downto 4))))(7 downto 0) <= unsigned(data_in);
                        when "0011" => voices_octave_carrier(to_integer(unsigned(addr(5 downto 4))))(3 downto 0) <= unsigned(data_in(3 downto 0));
                        when "0100" => voices_gain_message(to_integer(unsigned(addr(5 downto 4))))(5 downto 0) <= unsigned(data_in(5 downto 0));
                                       voices_waveform_message(to_integer(unsigned(addr(5 downto 4)))) <= data_in(7 downto 6);
                        when "0101" => voices_gain_modulated(to_integer(unsigned(addr(5 downto 4))))(5 downto 0) <= unsigned(data_in(5 downto 0));
                                       voices_waveform_modulated(to_integer(unsigned(addr(5 downto 4)))) <= data_in(7 downto 6);
                        when "0110" => voices_dac_direct_value(to_integer(unsigned(addr(5 downto 4)))) <= data_in;
                        when "0111" => voices_attack_rate(to_integer(unsigned(addr(5 downto 4)))) <= unsigned(data_in(3 downto 0));
                                       voices_release_rate(to_integer(unsigned(addr(5 downto 4)))) <= unsigned(data_in(7 downto 4));
                        when others =>
                    end case;
                end if;
            else
                voices_reset_phase <= (others=>'0');
            end if;


            voices_phase_delta(0) <= voices_pitch(0)(11 downto 0);
            voices_phase_delta(1) <= voices_pitch(1)(11 downto 0);
            voices_phase_delta(2) <= voices_pitch(2)(11 downto 0);
            voices_phase_delta(3) <= voices_pitch(3)(11 downto 0);
            voices_octave_message(0) <= voices_pitch(0)(15 downto 12);
            voices_octave_message(1) <= voices_pitch(1)(15 downto 12);
            voices_octave_message(2) <= voices_pitch(2)(15 downto 12);
            voices_octave_message(3) <= voices_pitch(3)(15 downto 12);
        end if;
    end process;
    
    process(addr, voices_envelope_phase)
    begin
        case addr(3 downto 0) is
            when "0000" =>
                data_out(1 downto 0) <= voices_envelope_phase(to_integer(unsigned(addr(5 downto 4))));
                data_out(7 downto 2) <= (others=>'0');
            when others =>
                data_out <= (others=>'0');
        end case;
    end process;

end sound_arch;
	
