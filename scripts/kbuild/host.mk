# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Build host
# ==========================================================================
src := $(obj)

PHONY := _build
_build:

#
# Include Buildsystem function
include $(BUILD_HOME)/include/define.mk

#
# Read auto.conf if it exists, otherwise ignore
-include $(MAKE_HOME)/include/config/auto.conf

#
# Include obj makefile
build-dir := $(if $(filter /%,$(src)),$(src),$(MAKE_HOME)/$(src))
build-file := $(if $(wildcard $(build-dir)/Kbuild),$(build-dir)/Kbuild,$(build-dir)/Makefile)
include $(build-file)

#
# Include host rule
include $(BUILD_HOME)/modules/host_rule.mk

########################################
# HOSTCC options                       #
########################################

#
# Add dependent file				
hostc_flags		= -Wp,-MD,$(depfile)
hostcxx_flags	= -Wp,-MD,$(depfile)

########################################
# Start build                          #
########################################

#
# include rules: all auxiliary rule
include $(BUILD_HOME)/include/auxiliary.mk

# Create executable from a single .c file
# host-csingle -> Executable
quiet_cmd_host-csingle 	= $(ECHO_HOSTCC)  $@
      cmd_host-csingle	= $(HOSTCC) $(hostc_flags) -o $@ $< \
	  	$(KBUILD_HOSTLDLIBS) $(HOSTLDLIBS_$(@F))
$(host-csingle): $(obj)/%: $(src)/%.c FORCE
	$(call if_changed_dep,host-csingle)

# Link an executable based on list of .o files, all plain c
# host-cmulti -> executable
quiet_cmd_host-cmulti	= $(ECHO_HOSTLD)  $@
      cmd_host-cmulti	= $(HOSTCC) $(KBUILD_HOSTLDFLAGS) -o $@ \
			  $(addprefix $(obj)/,$($(@F)-objs)) \
			  $(KBUILD_HOSTLDLIBS) $(HOSTLDLIBS_$(@F))
$(host-cmulti): FORCE
	$(call if_changed,host-cmulti)
$(call multi_depend, $(host-cmulti), , -objs)

# Create .o file from a single .c file
# host-cobjs -> .o
quiet_cmd_host-cobjs	= $(ECHO_HOSTCC) $@
      cmd_host-cobjs	= $(HOSTCC) $(hostc_flags) -c -o $@ $<
$(host-cobjs): $(obj)/%.o: $(src)/%.c FORCE
	$(call if_changed_dep,host-cobjs)

# Link an executable based on list of .o files, a mixture of .c and .cc
# host-cxxmulti -> executable
quiet_cmd_host-cxxmulti	= $(ECHO_HOSTLD) $@
      cmd_host-cxxmulti	= $(HOSTCXX) $(KBUILD_HOSTLDFLAGS) -o $@ \
			  $(foreach o,objs cxxobjs,\
			  $(addprefix $(obj)/,$($(@F)-$(o)))) \
			  $(KBUILD_HOSTLDLIBS) $(HOSTLDLIBS_$(@F))
$(host-cxxmulti): FORCE
	$(call if_changed,host-cxxmulti)
$(call multi_depend, $(host-cxxmulti), , -objs -cxxobjs)

# Create .o file from a single .cc (C++) file
quiet_cmd_host-cxxobjs	= $(ECHO_HOSTCXX) $@
      cmd_host-cxxobjs	= $(HOSTCXX) $(hostcxx_flags) -c -o $@ $<
$(host-cxxobjs): $(obj)/%.o: $(src)/%.cc FORCE
	$(call if_changed_dep,host-cxxobjs)

# Compile .c file, create position independent .o file
# host-cshobjs -> .o
quiet_cmd_host-cshobjs	= $(ECHO_HOSTCC)  -fPIC $@
      cmd_host-cshobjs	= $(HOSTCC) $(hostc_flags) -fPIC -c -o $@ $<
$(host-cshobjs): $(obj)/%.o: $(src)/%.c FORCE
	$(call if_changed_dep,host-cshobjs)

# Compile .c file, create position independent .o file
# Note that plugin capable gcc versions can be either C or C++ based
# therefore plugin source files have to be compilable in both C and C++ mode.
# This is why a C++ compiler is invoked on a .c file.
# host-cxxshobjs -> .o
quiet_cmd_host-cxxshobjs	= $(ECHO_HOSTCXX) -fPIC $@
      cmd_host-cxxshobjs	= $(HOSTCXX) $(hostcxx_flags) -fPIC -c -o $@ $<
$(host-cxxshobjs): $(obj)/%.o: $(src)/%.c FORCE
	$(call if_changed_dep,host-cxxshobjs)

# Link a shared library, based on position independent .o files
# *.o -> .so shared library (host-cshlib)
quiet_cmd_host-cshlib	= $(ECHO_HOSTLLD) -shared $@
      cmd_host-cshlib	= $(HOSTCC) $(HOSTLDFLAGS) -shared -o $@ \
			  $(addprefix $(obj)/,$($(@F:.so=-objs))) \
			  $(HOST_LOADLIBES) $(HOSTLDLIBS_$(@F))
$(host-cshlib): FORCE
	$(call if_changed,host-cshlib)
$(call multi_depend, $(host-cshlib), .so, -objs)

# Link a shared library, based on position independent .o files
# *.o -> .so shared library (host-cxxshlib)
quiet_cmd_host-cxxshlib	= $(ECHO_HOSTLLD) -shared $@
      cmd_host-cxxshlib	= $(HOSTCXX) $(HOSTLDFLAGS) -shared -o $@ \
			  $(addprefix $(obj)/,$($(@F:.so=-objs))) \
			  $(HOST_LOADLIBES) $(HOSTLDLIBS_$(@F))
$(host-cxxshlib): FORCE
	$(call if_changed,host-cxxshlib)
$(call multi_depend, $(host-cxxshlib), .so, -objs)

########################################
# Start build                          #
########################################

_build: $(always-y) $(subdir-y)

########################################
# Descending build                     #
########################################

PHONY += $(subdir-y)
$(subdir-y):
	$(Q)$(MAKE) $(build)=$@

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
