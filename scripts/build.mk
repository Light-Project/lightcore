# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Main build system
# ==========================================================================
src := $(obj)

PHONY := _build
_build:

obj-y := 
obj-m := 
obj-n := 

#
# Read auto.conf if it exists, otherwise ignore
-include include/config/auto.conf

#
# Include obj makefile
build-dir := $(if $(filter /%,$(src)),$(src),$(srctree)/$(src))
build-file := $(if $(wildcard $(build-dir)/Kbuild),$(build-dir)/Kbuild,$(build-dir)/Makefile)
include $(build-file)

#
# Include Buildsystem function
include $(srctree)/scripts/include.mk

#
# Include main rule
include $(srctree)/scripts/rule.mk

include scripts/auxiliary/pack.mk

########################################
# Include File                         #
########################################

include_blend   := $(include-y)

include_file    := $(filter %.h, $(include_blend))
include_path    := $(filter %/, $(include_blend))
include_path    := $(patsubst %/,%,$(include_path))

include_file    := $(addprefix -include ,$(include_file))
include_path    := $(addprefix -I ,$(include_path))

########################################
# OBJ options                          #
########################################

a_flags		= $(asflags-y) $(acflags-y) $(gcc-warning) -Wp,-MD,$(depfile) $(include_path) $(include_file)

c_flags		= $(ccflags-y) $(acflags-y) $(gcc-warning) -Wp,-MD,$(depfile) $(include_path) $(include_file)

cxx_flags	= $(cxxflags-y) $(gcc-warning) -Wp,-MD,$(depfile) $(include_path)  $(include_file)

cpp_flags	= $(cppflags-y) $(gcc-warning) -Wp,-MD,$(depfile) $(include_path) $(include_file)

lds_flags	= $(cppflags-y) $(ldsflags-y) $(gcc-warning) -Wp,-MD,$(depfile) $(include_path) $(include_file)

ld_flags	= $(LDFLAGS) $(ldflags-y)

export asflags-y ccflags-y cppflags-y acflags-y ldsflags-y ldflags-y

ifeq ($(BUILD_ENABLE_EXTRA_GCC_DEBUG),1)
a_flags         += -g
c_flags         += -g
cxx_flags       += -g
endif

########################################
# Start rule                           #
########################################

# Do not include hostprogs rules unless needed.
# $(sort ...) is used here to remove duplicated words and excessive spaces.
hostprogs := $(sort $(hostprogs))
ifneq ($(hostprogs),)
include scripts/host.mk
endif

# Compile C++ sources (.cpp)
# ---------------------------------------------------------------------------


quiet_cmd_cxx_o_cpp = $(ECHO_CXX) $@
	  cmd_cxx_o_cpp = $(CXX) $(cxx_flags) -c -o $@ $<
$(obj)/%.o: $(src)/%.cpp FORCE
	$(call if_changed_dep,cxx_o_cpp)


# Compile C sources (.c)
# ---------------------------------------------------------------------------

quiet_cmd_cc_s_c = $(ECHO_CC) $@
	  cmd_cc_s_c = $(CC) $(c_flags) -fverbose-asm -S -o $@ $<
$(obj)/%.s: $(src)/%.c FORCE
	$(call if_changed_dep,cc_s_c)

quiet_cmd_cc_i_c = $(ECHO_CPP) $@
	  cmd_cc_i_c = $(CPP) $(c_flags)   -o $@ $<
$(obj)/%.i: $(src)/%.c FORCE
	$(call if_changed_dep,cc_i_c)

# C (.c) files
# The C file is compiled and updated dependency information is generated.
# (See cmd_cc_o_c + relevant part of rule_cc_o_c)

quiet_cmd_cc_o_c = $(ECHO_CC) $@
	  cmd_cc_o_c = $(CC) $(c_flags) -c -o $@ $<
$(obj)/%.o: $(src)/%.c FORCE
	$(call if_changed_dep,cc_o_c)

quiet_cmd_cc_lst_c = MKLST   $@
      cmd_cc_lst_c = $(CC) $(c_flags) -g -c -o $*.o $< && \
		     $(CONFIG_SHELL) $(srctree)/scripts/makelst $*.o \
				     System.map $(OBJDUMP) > $@
$(obj)/%.lst: $(src)/%.c FORCE
	$(call if_changed_dep,cc_lst_c)

# Compile assembler sources (.S)
# ---------------------------------------------------------------------------

quiet_cmd_as_s_S	= $(ECHO_CPP) $@
	  cmd_as_s_S	= $(CPP) $(a_flags) -o $@ $< 

$(obj)/%.s: $(src)/%.S FORCE
	$(call if_changed_dep,as_s_S)

quiet_cmd_as_o_S = $(ECHO_AS) $@
	  cmd_as_o_S       = $(AS) $(a_flags) -c -o $@ $<

$(obj)/%.o: $(src)/%.S FORCE
	$(call if_changed_dep,as_o_S)


# Linker scripts preprocessor (.lds.S -> .lds)
# ---------------------------------------------------------------------------
quiet_cmd_cpp_lds_S = $(ECHO_LDS) $@
      cmd_cpp_lds_S = $(CPP) $(lds_flags) -P -o $@ $<

$(obj)/%.lds: $(src)/%.lds.S FORCE
	$(call if_changed_dep,cpp_lds_S) 

#
# Rule to compile a set of .o files into one .o file
$(obj-subfile):$(subdir-y)
quiet_cmd_link_o_target = $(ECHO_AR) $@
#       cmd_link_o_target = $(if $(strip $(obj-file) $(obj-subfile) $(subdir-y)),\
#                         $(LD) $(ld_flags) -r -o $@ $(obj-file) $(obj-subfile), \
#                         rm -f $@; $(AR) cDPrST $@)
        cmd_link_o_target = rm -f $@; $(AR) cDPrST $@ $(obj-file) $(obj-subfile)
$(builtin-target): $(obj-file) $(obj-subfile) FORCE
	$(call if_changed,link_o_target)

#
# Rule to compile a set of .o files into one .a file
$(lib-subfile):$(subdir-y)
quiet_cmd_link_l_target = $(ECHO_AR) $@
      cmd_link_l_target = rm -f $@; $(AR) rcs $(KBUILD_ARFLAGS) $@ $(lib-file) $(lib-subfile)
$(library-target): $(lib-file) $(lib-subfile) FORCE
	$(call if_changed,link_l_target)

########################################
# Start build                          #
########################################

_build: $(subdir-y) $(always_y)

# Descending
# ---------------------------------------------------------------------------

PHONY += $(subdir-y)
$(subdir-y):
	$(Q)$(MAKE) $(build)=$@

# Add FORCE to the prequisites of a target to force it to be always rebuilt.
# ---------------------------------------------------------------------------

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
