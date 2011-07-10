--===========================================================================--
--                                                                           --
--  bit_funcs.vhd - Power2 & Log2 Funtions Package                           --
--                                                                           --
--===========================================================================--
--
--  File name      : bit_funcs.vhd
--
--  Purpose        : Implements power2 and log2 functions.
--                  
--  Dependencies   : ieee.std_logic_1164
--                   ieee.std_logic_arith
--                   ieee.std_logic_unsigned
--
--  Author         : John E. Kent
--
--  Email          : dilbert57@opencores.org      
--
--  Web            : http://opencores.org/project,system09
--
--  bit_func.vhd is a VHDL functions package for calulating power2 and log2.
-- 
--  Copyright (C) 2003 - 2010 John Kent
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
--===========================================================================--
--                                                                           --
--                              Revision  History                            --
--                                                                           --
--===========================================================================--
--
-- Revision Name          Date             Description
-- 0.1      John E. Kent  unknown          Initial version
-- 1.0      John E. Kent  30th May 2010    Added GPL Header
--

library IEEE;
   use IEEE.std_logic_1164.all;
   use IEEE.std_logic_arith.all;
   use IEEE.std_logic_unsigned.all;

package bit_funcs is
   function log2(v: in natural) return natural;
   function pow2(v: in natural) return natural;
end package bit_funcs;

package body bit_funcs is


   function log2 (v : in natural) return natural is
   variable temp, log: natural;
   begin
      temp := v / 2;
      log := 0;
      while (temp /= 0) loop
        temp := temp/2;
        log := log + 1;
      end loop;
      return log;
   end function log2;

   function pow2(v: in natural) return natural is
      variable i: natural;
      variable pown: natural;
   begin
      pown := 1;
      for i in 0 to v loop
         exit when (i=v);
         pown := pown * 2;
      end loop;
      return pown;
   end function pow2;

end package body bit_funcs;
