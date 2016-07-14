-- MDSynth Sound Chip
--
-- Copyright (c) 2016, Meldora Inc.
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
-- ASDR Envelope
--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity envelope is
    port (  clk:		in std_logic;
            reset:	    in std_logic;
            note_on:    in std_logic;
            attack_rate:    in unsigned(3 downto 0);
            release_rate:   in unsigned(3 downto 0);
            gain:       out unsigned(5 downto 0));
end envelope;

architecture envelope_arch of envelope is

type phase_type is (
    wait_phase,
    attack_phase,
    sustain_phase,
    release_phase);

signal current_phase: phase_type := wait_phase;
signal current_gain: unsigned(25 downto 0); -- current_gain(25) is carry

begin

process (clk, reset)
begin
    if (reset = '1') then
        current_phase <= wait_phase;
        current_gain <= to_unsigned(0, 26);
    elsif (rising_edge(clk)) then
        gain <= current_gain(24 downto 19);
        case current_phase is
            when wait_phase =>
                if (note_on = '1') then
                    current_phase <= attack_phase;
                end if;
            when attack_phase =>
                if (note_on = '1') then
                    if (current_gain(25) = '1') then
                        current_gain <= to_unsigned(33554431, 26);
                        current_phase <= sustain_phase;
                    else
                        current_gain <= current_gain + attack_rate;
                    end if;
                else
                    current_phase <= release_phase;
                end if;
                
            when sustain_phase =>
                if (note_on = '0') then
                    current_phase <= release_phase;
                end if;
            when release_phase =>
                -- release
                if (note_on = '1') then
                    -- We have a note on during release phase
                    -- Start a new attack phase
                    current_gain <= to_unsigned(0, 26);
                    current_phase <= attack_phase;
                else
                    if (current_gain(25) = '1') then
                        current_gain <= to_unsigned(0, 26);
                        current_phase <= wait_phase;
                    else
                        current_gain <= current_gain - release_rate;
                    end if;
                end if;
        end case;
end if;

end process;

end envelope_arch;
