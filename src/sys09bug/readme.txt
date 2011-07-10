System09 Monitor Program 
========================
John Kent
3rd February 2008

1. Introduction.

System09 is FPGA SOC (System On a Chip) that uses a 6809 instruction compatible FPGA core.
The system is based around the old SWTPc (South West Technical Product Computer),
the difference being the FPGA version runs witha 25 MHz E Clock where as the SWTPc
ran with a 1MHz to 2MHz E clock.

2. Disk Operating Systems

The SWTPc ran the FLEX9, OS9 and Uniflex operating systems. 

2.1 Flex9

Flex is a single user operating system developed by TSC (Technical Systems Consultants).
It was to the Motorola 8 bit microprocessor family what CP/M was to the 8080 and Z80 family.
Flex9 typically used SBUG1.8 as the monitor program for providing the terminal drivers
and primative debug functions. 

2.2 UniFlex

UniFlex was TSC's multi tasking version of their Flex operating system.
I don't know much about it, other that the fact that much of the  code is available on the
internet. UniFlex made use of the extended memory, and used thw SWTPc DMAF-2 Floppy
Disk Controller.

2.3 OS9 

OS9 is a realtime multitasking operating system. It used to come in a basic 64K version
called Level 1 and an extended memory version that used the DAT (Dynamic Address Translation)
to map in up to 1 MByte of memory and was called level 2.

The SWTPc emulator from Evensons Consulting Services uses just the ACIA serial port
at $E000, a SWTPc MP-T timer module at $E040 for interrupts and a Western Digital
FD1771 or FD179X Floppy disk controller to run OS9 level 1.

OS9 had two special 2KByte boot ROMs at resided at $F000 and $F800.
These contain the start up code to load OS9 from the disk drive and run it.

3. Monitor Features

System09 is based on SBUG1.8, which was typically used for the Flex9 operating system
however I have had to strip out some of the functionality to fit video display and
PS/2 keyboard drivers. The VDU and PS/2 keyboard operate in parallel with the serial
port so that commands may be entered either locally on the FPGA system remotely by
the serial port on say a PC running a terminal program such as hyperterm. 

3.1 Mass storage

SBUG1.8 had two disk boots, one was for a the DC-1 or compatible floppy disk controller
that was mapped at $E01X and the other was for the DMAF2 which used the 6844 DMA controller 
and was mapped at $F000 just below the SBUG1.8 monitor program.

In system09 there is no Western Digital Compatible Floppy disk Controller. (At least not yet.)
Instead provisons have been made for booting from either a Compact Flash card or an IDE hard drive.

The method for interfaceing to an IDE drive or CF card varies from FPGA board to FPGA board.
Some FPGA boards such as the Spartan 3 and Spartan 3E starter boards do not have provision
for a Compact Flash card or IDE card, although they do have expansion ports that could be
used for CF or IDE interfaces.

Some FPGA boards, such as the BurchED B5-X300 have a dedicated CF add on module, which allows 
you to map the CF card in any way you like. The XESS XST-3S1000 and XST-3.0 carrier board on the
other hand provides a 16 bit peripheral bus that can be used to access the IDE drive connector, 
ethernet controller or two expansion connectors. The Trenz TE0141 and TE0140 boards have the
CF card mapped into the memory address and data bus, but have done so in such a fashion that it
can only be used in 8 bit transfer mode.

CF cards can be programmed to run in 8 bit mode or 16 bit mode. 8 Bit mode is handier for
interfaceing to 8 bit micros sych as system09, however this mode is not necessarily supported
by IDE disk drives.

4. FPGA Cards

4.1 BurchEd B5-X300

The B5-X300 version of Sys09bug is covered by the "FPGAOPT" options flag.
It provides text VDU drivers and PS/2 keyboard interface, ACIA serial port,
and Compact Flash Bootstrap. The B5-X300 uses a 300Kgate Spartan 2E FPGA which
means the Block RAM used to hold the monitor ROMs is only 4Kbits. 
four Block RAMs are required to implement a 2KByte monitor program.

4.2 Digilent / Xilinx Spartan 3 Starter Board

The Xilinx spartan 3 starter board was designed by Digilent. It uses the Spartan 3 FPGA
and intially came in a 200K gate version although a 1000KGate version is also available.
The Spartan 3 starterboard is fitted wit 256K x 32 bits of RAM, which is mapped as 1M x 8 bits. 

The Spartan 3 starter board does not have any mass storage interfaces, although it does have
expansion sockets that may be used to add external CF or IDE interfaces. The SRAM on the 
otherhand can be used as a RAM disk, and data can be downloaded via serial link. 

David Rumball designed his version of system09 called the Microbox 2000 which was based on
the spartan 3 starter board. He was able to use a configuration flash loader to load
a ROM disk into RAM. He also implemented a RAM disk and a virtual disk that was hosted on
a PC using the FLEXNET software.

I borrowed some of his work to extend sys09bug on the spartan 3 starter but have yet to get
it working.

4.3 Digilent / Xilinx Spartan 3E starter board 

Digilent produced a Spartan 3e starter board for Xilinx. The big difference between the
Spartan 3 board and the spartan 3E board is that the latter uses SDRAM rather than SRAM
and is consequently harder to use. The spartan 3E starter was used to verify the operation
of the 6809 core, however because of the difficulty implementing the SDRAM controller
I used Block RAM instead.  The design implemented a PIA interrupt timer that sys09bug could
use to single step through instructions one at a time. The S3E version of System09 implements 
a Trace command that performs a register dump after each instruction is executed.
This allowed comparison of the FPGA 6809 core with a reference design using the original chip.

Because RAM was limitted in the S3E, System09 also does not implement the DAT so the monitor
start up code at the top of memory to test for memory is not implemented in the S3E version.

4.4 XESS XSA-3S1000 / XST-3.0



