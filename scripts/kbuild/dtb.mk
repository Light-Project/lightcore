# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Build dtb
# ==========================================================================

src := $(obj)

PHONY := _build
_build:

# Read auto.conf if it exists, otherwise ignore
-include $(MAKE_HOME)/include/config/auto.conf

#
# Include obj makefile
build-dir := $(if $(filter /%,$(src)),$(src),$(MAKE_HOME)/$(src))
build-file := $(if $(wildcard $(build-dir)/Kbuild),$(build-dir)/Kbuild,$(build-dir)/Makefile)
include $(build-file)

#
# Include Buildsystem function
include $(BUILD_HOME)/include/define.mk

#
# Include dtb rule
include $(BUILD_HOME)/modules/dtb_rule.mk

########################################
# Start rule                           #
########################################

quiet_cmd_dts_o_dtb = $(ECHO_DTB) $@
	  cmd_dts_o_dtb = $(DTC) $< > $@
$(obj)/%.dtb: $(src)/%.dts FORCE
	$(call if_changed,dts_o_dtb)

########################################
# Start build                          #
########################################

_build: $(subdir-y) $(always-y)

########################################
# Descending build                     #
########################################

PHONY += $(subdir-y)
$(subdir-y):
	$(Q)$(MAKE) $(build)=$@

PHONY += FORCE 
FORCE:

# Read all saved command lines and dependencies for the $(targets) we
# may be building above, using $(if_changed{,_dep}). As an
# optimization, we don't need to read them if the target does not
# exist, we will rebuild anyway in that case.
targets := $(wildcard $(sort $(targets)))
cmd_files := $(wildcard $(foreach f,$(targets),$(dir $(f)).$(notdir $(f)).cmd))

ifneq ($(cmd_files),)
  include $(cmd_files)
endif

.PHONY: $(PHONY)
