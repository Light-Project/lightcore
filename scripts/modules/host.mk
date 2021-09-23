# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Build host
# ==========================================================================

# Include host rule
include $(build_home)/modules/host_rule.mk

#####################################
# HOSTCC options                    #
#####################################

_hostc_flags    = $(KBUILD_HOSTCFLAGS) $(HOST_EXTRACFLAGS) \
                  $(HOSTCFLAGS_$(target-stem).o)
_hostcxx_flags  = $(KBUILD_HOSTCXXFLAGS) $(HOST_EXTRACXXFLAGS) \
                  $(HOSTCXXFLAGS_$(target-stem).o)

hostc_flags     = -Wp,-MMD,$(depfile) $(_hostc_flags)
hostcxx_flags   = -Wp,-MMD,$(depfile) $(_hostcxx_flags)

#####################################
# Build rule                        #
#####################################

# LEX
quiet_cmd_flex = $(ECHO_LEX) $@
      cmd_flex = $(LEX) -o$@ -L $<
$(obj)/%.lex.c: $(src)/%.l FORCE
	$(call if_changed,flex)

# YACC
quiet_cmd_bison = $(ECHO_YACC) $(basename $@).[ch]
      cmd_bison = $(YACC) -o $(basename $@).c --defines=$(basename $@).h -t -l $<
$(obj)/%.tab.c $(obj)/%.tab.h: $(src)/%.y FORCE
	$(call if_changed,bison)

# Create executable from a single .c file
quiet_cmd_host-csingle = $(ECHO_HOSTCC)  $@
      cmd_host-csingle = $(HOSTCC) $(hostc_flags) -o $@ $< \
                         $(HOSTLDLIBS_$(target-stem))
$(host_csingle): $(obj)/%: $(src)/%.c FORCE
	$(call if_changed_dep,host-csingle)

# Link an executable based on list of .o files, all plain c
quiet_cmd_host-cmulti = $(ECHO_HOSTLD) $@
      cmd_host-cmulti = $(HOSTCC) $(KBUILD_HOSTLDFLAGS) -o $@ \
                        $(addprefix $(obj)/,$($(@F)-objs))    \
                        $(HOSTLDLIBS_$(target-stem))
$(host_cmulti): FORCE
	$(call if_changed,host-cmulti)
$(call multi_depend, $(host_cmulti), , -objs)

# Create .o file from a single .c file
quiet_cmd_host-cobjs = $(ECHO_HOSTCC) $@
      cmd_host-cobjs = $(HOSTCC) $(hostc_flags) -c -o $@ $<
$(host_cobjs): $(obj)/%.o: $(src)/%.c FORCE
	$(call if_changed_dep,host-cobjs)

# Link an executable based on list of .o files, a mixture of .c and .cc
quiet_cmd_host-cxxmulti = $(ECHO_HOSTLD) $@
      cmd_host-cxxmulti = $(HOSTCXX) $(KBUILD_HOSTLDFLAGS) -o $@ \
                           $(foreach o,objs cxxobjs,             \
                           $(addprefix $(obj)/,$($(@F)-$(o))))   \
                           $(HOSTLDLIBS_$(target-stem))
$(host_cxxmulti): FORCE
	$(call if_changed,host-cxxmulti)
$(call multi_depend, $(host-cxxmulti), , -objs -cxxobjs)

# Create .o file from a single .cc (C++) file
quiet_cmd_host-cxxobjs = $(ECHO_HOSTCXX) $@
      cmd_host-cxxobjs = $(HOSTCXX) $(hostcxx_flags) -c -o $@ $<
$(host_cxxobjs): $(obj)/%.o: $(src)/%.cc FORCE
	$(call if_changed_dep,host-cxxobjs)

# Compile .c file, create position independent .o file
quiet_cmd_host-cshobjs = $(ECHO_HOSTCC) $@
      cmd_host-cshobjs = $(HOSTCC) $(hostc_flags) -fPIC -c -o $@ $<
$(host_cshobjs): $(obj)/%.o: $(src)/%.c FORCE
	$(call if_changed_dep,host-cshobjs)

# Compile .c file, create position independent .o file
# Note that plugin capable gcc versions can be either C or C++ based
# therefore plugin source files have to be compilable in both C and C++ mode.
# This is why a C++ compiler is invoked on a .c file.
quiet_cmd_host-cxxshobjs = $(ECHO_HOSTCXX) $@
      cmd_host-cxxshobjs = $(HOSTCXX) $(hostcxx_flags) -fPIC -c -o $@ $<
$(host_cxxshobjs): $(obj)/%.o: $(src)/%.c FORCE
	$(call if_changed_dep,host-cxxshobjs)

# Link a shared library, based on position independent .o files
quiet_cmd_host-cshlib = $(ECHO_HOSTLLD) $@
      cmd_host-cshlib = $(HOSTCC) $(HOSTLDFLAGS) -shared -o $@  \
                        $(addprefix $(obj)/,$($(@F:.so=-objs))) \
                        $(HOST_LOADLIBES) $(HOSTLDLIBS_$(@F))
$(host_cshlib): FORCE
	$(call if_changed,host-cshlib)
$(call multi_depend, $(host_cshlib), .so, -objs)

# Link a shared library, based on position independent .o files
quiet_cmd_host-cxxshlib = $(ECHO_HOSTLLD) $@
      cmd_host-cxxshlib = $(HOSTCXX) $(HOSTLDFLAGS) -shared -o $@ \
                          $(addprefix $(obj)/,$($(@F:.so=-objs))) \
                          $(HOST_LOADLIBES) $(HOSTLDLIBS_$(@F))
$(host_cxxshlib): FORCE
	$(call if_changed,host-cxxshlib)
$(call multi_depend, $(host_cxxshlib), .so, -objs)
