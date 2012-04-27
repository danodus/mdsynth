-- MDSynth Sound Chip
--
-- Copyright (c) 2012, Meldora Inc.
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
-- Base + 0: Waveform (0x00: None, 0x01: Square, 0x02: Sawtooth, 0x03: Sine, 0x04: Phase Modulation)
-- Base + 1: Pitch Hi (see below)
-- Base + 2: Pitch Low (see below)
-- Base + 3: Carrier Octave (4-bit, PM only)
-- Base + 4: Message Gain (6-bit, sine and PM only)
-- Base + 5: Modulated Gain (6-bit, PM only)
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
    addr       : in  std_logic_vector(2 downto 0);
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
           waveform: in std_logic_vector(2 downto 0);    -- 0: None, 1: Square (message only), 2: Sawtooth (message only), 3: Sine (message only), 4: FM (implemented as phase modulation)
           gain_message:          in unsigned(5 downto 0);
           gain_modulated:        in unsigned(5 downto 0);
           phase_delta:           in unsigned(11 downto 0);
           octave_message:        in unsigned(3 downto 0);
           octave_carrier:        in unsigned(3 downto 0);
           output:   out std_logic);
end component;

signal waveform : std_logic_vector(2 downto 0);

signal pitch: unsigned(15 downto 0);

signal phase_delta: unsigned(11 downto 0);
signal octave_message: unsigned(3 downto 0);
signal octave_carrier: unsigned(3 downto 0);

signal gain_message: unsigned(5 downto 0) := to_unsigned(63, 6);
signal gain_modulated: unsigned(5 downto 0) := to_unsigned(63, 6);

begin

    channel0 : channel port map (clk => clk_50, reset => rst, waveform => waveform, gain_message => gain_message, gain_modulated => gain_modulated, phase_delta => phase_delta, octave_message => octave_message, octave_carrier => octave_carrier, output => audio_out);

    process(clk)
    begin
        if clk'event and clk = '0' then
            if rst = '1' then
                pitch <= to_unsigned(0, 16);
                waveform <= (others=>'0');
            elsif cs = '1' and rw = '0' then
                case addr is
                    when "000" => waveform <= data_in(2 downto 0);
                    when "001" => pitch(15 downto 8) <= unsigned(data_in);
                    when "010" => pitch(7 downto 0) <= unsigned(data_in);
                    when "011" => octave_carrier(3 downto 0) <= unsigned(data_in(3 downto 0));
                    when "100" => gain_message(5 downto 0) <= unsigned(data_in(5 downto 0));
                    when "101" => gain_modulated(5 downto 0) <= unsigned(data_in(5 downto 0));
                    when others => waveform <= (others=>'0');
                end case;
            end if;
            phase_delta <= pitch(11 downto 0);
            octave_message <= pitch(15 downto 12);
        end if;
    end process;

end sound_arch;
	
