# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Main build system
# ==========================================================================
src := $(obj)

PHONY := _build
_build:

#
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
# Include Buildsystem function
include $(BUILD_HOME)/tools/image.mk

#
# Include main rule
include $(BUILD_HOME)/main/main_rule.mk

########################################
# Include File                         #
########################################

include_file    := $(filter %.h, $(INCLUDE))

include_path    := $(filter %/, $(INCLUDE))
include_path    := $(patsubst %/,%,$(include_path))

include_file    := $(addprefix -include ,$(include_file))
include_path    := $(addprefix -I ,$(include_path))

########################################
# OBJ options                          #
########################################

ifeq ($(asflags-y),)
asflags-y	:= $(asflags-sub-y)
endif

ifeq ($(ccflags-y),)
ccflags-y	:= $(ccflags-sub-y)
endif

ifeq ($(cppflags-y),)
cppflags-y	:= $(cppflags-sub-y)
endif

ifeq ($(acflags-y),)
acflags-y	:= $(acflags-sub-y)
endif

ifeq ($(ldflags-y),)
ldflags-y	:= $(ldflags-sub-y)
endif

export asflags-sub-y ccflags-sub-y cppflags-sub-y acflags-sub-y ldflags-sub-y

ifeq ($(BUILD_ENABLE_EXTRA_GCC_DEBUG),1)
asflags-y	+= -g
ccflags-y	+= -g
cxxflags-y	+= -g
endif

a_flags		= $(asflags-y) $(acflags-y) $(gcc-warning) -Wp,-MD,$(depfile) $(include_path) $(include_file)

c_flags		= $(ccflags-y) $(acflags-y) $(gcc-warning) -Wp,-MD,$(depfile) $(include_path) $(include_file)

cxx_flags	= $(cxxflags-y) $(gcc-warning) -Wp,-MD,$(depfile) $(include_path)  $(include_file)

cpp_flags	= $(cppflags-y) $(gcc-warning) -Wp,-MD,$(depfile) $(include_path) $(include_file)

lds_flags	= $(cppflags-y) $(gcc-warning) -Wp,-MD,$(depfile) $(include_path)

ld_flags	= $(LDFLAGS) $(ldflags-y)

########################################
# Start rule                           #
########################################

#
# include rules: all auxiliary rule
include $(BUILD_HOME)/include/auxiliary.mk

# include process: all auxiliary rule
include $(BUILD_HOME)/generic/process.mk


$(obj-subfile): $(subdir-y)

#
# Rule to compile a set of .o files into one .o file
quiet_cmd_link_o_target = $(ECHO_LD) $@
	  cmd_link_o_target = rm -f $@; $(AR) cDPrST $@ $(obj-file) $(obj-subfile)
$(builtin-target): $(obj-file) $(obj-subfile) FORCE
	$(call if_changed,link_o_target)

########################################
# Start build                          #
########################################

_build: $(subdir-y) $(always-y)

########################################
# Descending build                     #
########################################

PHONY += $(subdir-y)
$(subdir-y):
	$(Q)$(MAKE) $(build_main)=$@

########################################
# Start FORCE                          #
########################################

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
