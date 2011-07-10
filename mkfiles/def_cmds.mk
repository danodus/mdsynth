#-----------------------------------------------------------------
# File:    Makefile
# Author:  David Burnette
# Date:    April 7, 2008
#
# Description:
#  This makefile fragment defines common commands used by the
#  the makefiles. Vendor-specific tools should be defined in
#  the vendor-specific makefiles.
#
# Usage:
#  This make file fragment should be included by the 
#  makefiles in the 'rtl' and 'src/...' directories. 
#
# Dependencies:
#  None
#
# Revision History:
#   dgb 2008-04-07   Original version
#
#-----------------------------------------------------------------

#===================================================================

# DOS version of Unix-ish tools
CD       := cd
CP       := cp
CAT      := type
ECHO     := echo
RM       := erase /s /q
RMDIR    := rmdir /s /q
MKDIR    := mkdir

#  6809 Assembler
ASM      := ..\..\tools\as09\as09.exe

#  Generate Xilinx block ram initialized with ROM contents
s19tovhd := ..\..\tools\s19tovhd\S19toVHD.exe

