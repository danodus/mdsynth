#-----------------------------------------------------------------
# File:    xilinx_rules.mk
# Author:  David Burnette
# Date:    April 7, 2008
#
# Description:
#
# Usage:
#  This make file fragment contains translate rules for synthesizing
#  Xilinx designs.
#
# This work was based on the Xilinx Makefile by Dave Vanden Bout
# from XESS Corp. Several major differences exist between his
# implementation and mine. This Makefile does not require PERL
# (though it does require AWK). 
#
# Dependencies:
#  Depends on 'def_rules.mk' fragment.
#
# Revision History:
#   dgb 2008-04-07   Original version
#
#-----------------------------------------------------------------


include $(MKFRAGS)/def_rules.mk

# Determine Xilinx ISE location
ifeq "$(XILINX)" ""
  $(error The XILINX environment variable must be set to use this makefile)
endif
XILINX_NORMAL := $(subst \,/,$(XILINX))

MY_OS := $(shell uname -s)
ifeq "$(findstring CYGWIN_NT,$(MY_OS))" "CYGWIN_NT"
  ifeq "$(findstring WOW64,$(MY_OS))" "WOW64"
    XILINX_PLAT = nt64
  else
    XILINX_PLAT = nt
  endif
else
  ifeq "$(MY_OS)" "Linux"
    XILINX_PLAT = lin
  else
    $(error Could not determine OS type for locating XILINX applications)
  endif
endif

# Determine XILINX ISE version
XILINX_FILESET := $(XILINX_NORMAL)/fileset.txt
XILINX_VER := $(shell $(AWK) 'BEGIN { FS = "=" } /version/ { printf("%s",$$2) }'  $(XILINX_FILESET))
XILINX_MAJOR_VER := $(basename $(XILINX_VER))

# Xilinx tools
XST        := $(XILINX_NORMAL)/bin/$(XILINX_PLAT)/xst
NGDBUILD   := $(XILINX_NORMAL)/bin/$(XILINX_PLAT)/ngdbuild
MAP        := $(XILINX_NORMAL)/bin/$(XILINX_PLAT)/map
PAR        := $(XILINX_NORMAL)/bin/$(XILINX_PLAT)/par
BITGEN     := $(XILINX_NORMAL)/bin/$(XILINX_PLAT)/bitgen
PROMGEN    := $(XILINX_NORMAL)/bin/$(XILINX_PLAT)/promgen
TRCE       := $(XILINX_NORMAL)/bin/$(XILINX_PLAT)/trce
IMPACT     := $(XILINX_NORMAL)/bin/$(XILINX_PLAT)/impact

# Extract info from Xilinx ISE project for use with command line tools
XST_FILE := $(DESIGN_NAME).xst
PRJ_FILE := $(shell $(AWK) '/^-ifn/ { printf("%s",$$2) }'  $(XST_FILE))
HDL_FILES := $(subst ",,$(shell $(AWK) '{ print $$3} ' $(PRJ_FILE)))
PART := $(shell $(AWK) '/^-p / { printf("%s",$$2) }' $(XST_FILE))
DEVICE_tmp := $(shell $(AWK) -F - '/^-p / { printf("%s",$$2) }' $(XST_FILE))
DEVICE := $(subst p ,,$(DEVICE_tmp))
SPEED := $(shell $(AWK) -F - '/^-p / { printf("%s",$$3) }' $(XST_FILE))
PACKAGE := $(shell $(AWK) -F - '/^-p / { printf("%s",$$4) }' $(XST_FILE))
BSD_FILE := $(XILINX)/$(FAMILY)/data/$(DEVICE).bsd

XSTHDPDIR1 := $(shell $(AWK) '/^set -xsthdpdir / { printf("%s",$$3) }'  $(XST_FILE))
XSTHDPDIR := $(subst ",,$(XSTHDPDIR1))
TMPDIR1 := $(shell $(AWK) '/^set -tmpdir / { printf("%s",$$3) }'  $(XST_FILE))
TMPDIR := $(subst ",,$(TMPDIR1))

INTSTYLE         ?= -intstyle silent      # call Xilinx tools in silent mode
INTSTYLE :=
XST_FLAGS        ?= $(INTSTYLE)           # most synthesis flags are specified in the .xst file
NGDBUILD_FLAGS   ?= $(INTSTYLE) -dd _ngo  # ngdbuild flags
NGDBUILD_FLAGS += $(if $(UCF_FILE),-uc,) $(UCF_FILE)
# pre-11.1 flags
ifeq "$(XILINX_MAJOR_VER)" "11"
# ISE 11.1 flags
MAP_FLAGS        ?= $(INTSTYLE) -cm area -pr b -c 100 -tx off
else
MAP_FLAGS        ?= $(INTSTYLE) -cm area -pr b -k 4 -c 100 -tx off
endif
PAR_FLAGS        ?= $(INTSTYLE) -w -ol std -t 1
TRCE_FLAGS       ?= $(INTSTYLE) -e 3 -l 3
BITGEN_FLAGS     ?= $(INTSTYLE)           # most bitgen flags are specified in the .ut file
PROMGEN_FLAGS    ?= -u 0                  # flags that control the MCS/EXO file generation

BITGEN_OPTIONS_FILE   ?= $(DESIGN_NAME).ut

#===================================================================
# Debug variable settings
debug_vars:
	@$(ECHO) "XST_FILE    = '$(XST_FILE)'"
	@$(ECHO) "PRJ_FILE    = '$(PRJ_FILE)'"
	@$(ECHO) "HDL_FILES   = '$(HDL_FILES)'"
	@$(ECHO) "PART        = '$(PART)'"
	@$(ECHO) "DEVICE      = '$(DEVICE)'"
	@$(ECHO) "SPEED       = '$(SPEED)'"
	@$(ECHO) "PACKAGE     = '$(PACKAGE)'"
	@$(ECHO) "UCF_FILE    = '$(UCF_FILE)'"
	@$(ECHO) "BSD_FILE    = '$(BSD_FILE)'"
	@$(ECHO) "XSTHDPDIR   = '$(XSTHDPDIR)'"
	@$(ECHO) "TMPDIR      = '$(TMPDIR)'"

#===================================================================
# Make sure tmpdirs are created
xst_tmp_dirs:
	@$(MKDIR) $(XSTHDPDIR)
	@$(MKDIR) $(TMPDIR)
	touch $@

#===================================================================
# Define dependencies

$(DESIGN_NAME).ngc: $(XST_FILE) $(PRJ_FILE) xst_tmp_dirs $(HDL_FILES) $(DESIGN_NAME).lso

$(DESIGN_NAME).ngd: $(DESIGN_NAME).ngc $(UCF_FILE)

$(DESIGN_NAME).bit: $(DESIGN_NAME).ncd $(BITGEN_OPTIONS_FILE)

$(DESIGN_NAME).mcs: $(DESIGN_NAME).bit

#===================================================================
# Rule to make impact cmd file

$(DESIGN_NAME)_impact.cmd:
	@$(ECHO)
	@$(ECHO) "======= Generating Impact command file ============"
	-@$(RM) $(DESIGN_NAME)_impact.cmd
	@$(ECHO) "setMode -ss" >>$@
	@$(ECHO) "setMode -sm" >>$@
	@$(ECHO) "setMode -hw140" >>$@
	@$(ECHO) "setMode -spi" >>$@
	@$(ECHO) "setMode -acecf" >>$@
	@$(ECHO) "setMode -acempm" >>$@
	@$(ECHO) "setMode -pff" >>$@
	@$(ECHO) "setMode -bs" >>$@
	@$(ECHO) "setMode -bscan" >>$@
	@$(ECHO) "setCable -p auto" >>$@
	@$(ECHO) "addDevice -p 1 -file $(BSD_FILE)" >>$@
	@$(ECHO) "Identify" >>$@
	@$(ECHO) "identifyMPM" >>$@
	@$(ECHO) "assignFile -p 1 -file $(DESIGN_NAME).bit" >>$@
	@$(ECHO) "program -p 1" >>$@
	@$(ECHO) "quit" >>$@
	@$(CAT) $@

.PHONY: do_impact
do_impact: $(DESIGN_NAME).bit $(DESIGN_NAME).ngc $(DESIGN_NAME)_impact.cmd
	@$(ECHO)
	@$(ECHO) "======= Downloading bitstream to XSA-3S1000 using Impact ============"
	$(IMPACT) -batch $(DESIGN_NAME)_impact.cmd

#===================================================================
# TRANSLATE RULES

#   RULE: .xst => .ngc
# Synthesize the HDL files into an NGC file.  This rule is triggered if
# any of the HDL files are changed or the synthesis options are changed.
%.ngc: %.xst
	@$(ECHO)
	@$(ECHO) "======= Synthesis - XST ============================"
	$(XST) $(XST_FLAGS) -ifn $(XST_FILE) -ofn $(DESIGN_NAME).syr

#   RULE: .ngc => .ngd
# Take the output of the synthesizer and create the NGD file.  This rule
# will also be triggered if constraints file is changed.
%.ngd: %.ngc
	@$(ECHO)
	@$(ECHO) "======= Synthesis - NGDBUILD ======================="
	$(NGDBUILD) $(NGDBUILD_FLAGS) -p $(PART) $*.ngc $*.ngd

#   RULE: .ngd => _map.ncd and .pcf
# Map the NGD file and physical-constraints to the FPGA to create the mapped NCD file.
%_map.ncd %.pcf: %.ngd
	@$(ECHO)
	@$(ECHO) "======= Synthesis - MAP ============================"
	$(MAP) $(MAP_FLAGS) -p $(PART) -o $*_map.ncd $*.ngd $*.pcf

#   RULE: _map.ncd and .pcf => .ncd
# Place & route the mapped NCD file to create the final NCD file.
%.ncd: %_map.ncd %.pcf
	@$(ECHO)
	@$(ECHO) "======= Synthesis - PAR ============================"
	$(PAR) $(PAR_FLAGS) $*_map.ncd $*.ncd $*.pcf

#   RULE: .ncd => .bit
# Take the final NCD file and create an FPGA bitstream file.  This rule will also be
# triggered if the bit generation options file is changed.
%.bit: %.ncd $(BITGEN_OPTIONS_FILE)
	@$(ECHO)
	@$(ECHO) "======= Generating bitstream ======================="
	$(BITGEN) $(BITGEN_FLAGS) -f $(BITGEN_OPTIONS_FILE) $*.ncd

#   RULE: .bit => .mcs
# Convert a bitstream file into an MCS hex file that can be stored into Flash memory.
%.mcs: %.bit
	@$(ECHO)
	@$(ECHO) "======= Generating MCS prom ========================"
	$(PROMGEN) $(PROMGEN_FLAGS) $*.bit -p mcs -w

#   RULE: .bit => .exo
# Convert a bitstream file into an EXO hex file that can be stored into Flash memory.
%.exo: %.bit
	@$(ECHO)
	@$(ECHO) "======= Generating EXO prom ========================"
	$(PROMGEN) $(PROMGEN_FLAGS) $*.bit -p exo

# Create the FPGA timing report after place & route.
%.twr: %.ncd %.pcf
	@$(ECHO)
	@$(ECHO) "======= Generating Timing Report ==================="
	$(TRCE) $(TRCE_FLAGS) $*.ncd -o $*.twr $*.pcf

# Preserve intermediate files.
.PRECIOUS: %.ngc %.ngd %_map.ncd %.ncd %.twr %.vm6 %.jed


