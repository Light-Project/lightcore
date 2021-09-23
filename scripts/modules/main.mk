# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# Main build system
# ==========================================================================

# Include main rule
include $(build_home)/modules/main_rule.mk

include_file    := $(filter %.h,$(include_blend))
include_path    := $(filter %/,$(include_blend))
include_path    := $(patsubst %/,%,$(include_path))

include_file    := $(addprefix -include ,$(include_file))
include_path    := $(addprefix -I ,$(include_path))

########################################
# OBJ options                          #
########################################

a_flags     = -Wp,-MMD,$(depfile) $(asflags-y)  $(gcc-warning) $(include_path) $(include_file)
c_flags     = -Wp,-MMD,$(depfile) $(ccflags-y)  $(gcc-warning) $(include_path) $(include_file)
cxx_flags   = -Wp,-MMD,$(depfile) $(cxxflags-y) $(gcc-warning) $(include_path) $(include_file)
cpp_flags   = -Wp,-MMD,$(depfile) $(cppflags-y) $(gcc-warning) $(include_path) $(include_file)
lds_flags   = -Wp,-MMD,$(depfile) $(ldsflags-y) $(gcc-warning) $(include_path) $(include_file)
ld_flags    = -Wp,-MMD,$(depfile) $(ldflags-y)

a_flags     += $(acflags-y)
c_flags     += $(acflags-y)

export asflags-y ccflags-y cppflags-y acflags-y ldsflags-y ldflags-y

########################################
# Start rule                           #
########################################

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
quiet_cmd_as_s_S = $(ECHO_CPP) $@
	  cmd_as_s_S = $(CPP) $(a_flags) -o $@ $<
$(obj)/%.s: $(src)/%.S FORCE
	$(call if_changed_dep,as_s_S)

quiet_cmd_as_o_S = $(ECHO_AS) $@
	  cmd_as_o_S = $(AS) $(a_flags) -c -o $@ $<
$(obj)/%.o: $(src)/%.S FORCE
	$(call if_changed_dep,as_o_S)

# Linker scripts preprocessor (.lds.S -> .lds)
# ---------------------------------------------------------------------------
quiet_cmd_cpp_lds_S = $(ECHO_LDS) $@
      cmd_cpp_lds_S = $(CPP) $(lds_flags) -P -o $@ $<
$(obj)/%.lds: $(src)/%.lds.S FORCE
	$(call if_changed_dep,cpp_lds_S)

# Rule to compile a set of .o files into one .a file
quiet_cmd_link_a_target = $(ECHO_AR) $@
      cmd_link_a_target = rm -f $@; $(AR) cDPrST $@ $(obj_file) $(obj_subfile)
$(builtin_target): $(obj_file) $(obj_subfile) $(extra_file) FORCE
	$(call if_changed,link_a_target)

$(obj_subfile): $(obj)/%/built-in.a: $(obj)/% ;
