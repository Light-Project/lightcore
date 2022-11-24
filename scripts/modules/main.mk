# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# Main build system
# ==========================================================================

# Include main rule
include $(build_home)/include/kasan.mk
include $(build_home)/include/ubsan.mk
include $(build_home)/include/kgcov.mk
include $(build_home)/modules/main_rule.mk

include_file    := $(filter %.h,$(include_blend))
include_path    := $(filter %/,$(include_blend))
include_path    := $(patsubst %/,%,$(include_path))

include_file    := $(addprefix -include ,$(include_file))
include_path    := $(addprefix -I ,$(include_path))

########################################
# OBJ options                          #
########################################

acflags_y   += $(acflags-y)
cxflags_y   += $(cxflags-y)
asflags_y   += $(asflags-y)
ccflags_y   += $(ccflags-y)
cxxflags_y  += $(cxxflags-y)
ldsflags_y  += $(ldsflags-y)

a_flags     = $(gcc-warning) $(acflags_y) $(asflags_y) -Wp,-MD,$(depfile) $(include_path) $(include_file) $($(basetarget)-flags-y)
c_flags     = $(gcc-warning) $(acflags_y) $(cxflags_y) $(ccflags_y) -Wp,-MD,$(depfile) $(include_path) $(include_file) $($(basetarget)-flags-y)
cxx_flags   = $(gcc-warning) $(acflags_y) $(cxflags_y) $(cxxflags_y) -Wp,-MD,$(depfile) $(include_path) $(include_file) $($(basetarget)-flags-y)
cpp_flags   = $(gcc-warning) $(cppflags_y) -Wp,-MD,$(depfile) $(include_path) $(include_file) $($(basetarget)-flags-y)
lds_flags   = $(ldsflags_y) -Wp,-MD,$(depfile) $(include_path) $(include_file) $($(basetarget)-flags-y)

ifdef CONFIG_KASAN
c_flags += $(if $(patsubst n%,, \
            $(KASAN_SANITIZE_$(basetarget).o)$(KASAN_SANITIZE)$(CONFIG_KASAN_ALL)), \
            $(CFLAGS_KASAN), $(CFLAGS_KASAN_NOSANITIZE))
endif

ifdef CONFIG_UBSAN
c_flags += $(if $(patsubst n%,, \
            $(UBSAN_SANITIZE_$(basetarget).o)$(UBSAN_SANITIZE)$(CONFIG_UBSAN_ALL)), \
            $(CFLAGS_UBSAN))
endif

ifdef CONFIG_KGCOV
c_flags += $(if $(patsubst n%,, \
            $(UBSAN_PROFILE_$(basetarget).o)$(KGCOV_PROFILE)$(CONFIG_KGCOV_ALL)), \
            $(CFLAGS_KGCOV))
endif

unexport asflags-y cxflags-y ccflags-y cppflags-y acflags-y ldsflags-y
export acflags_y cxflags_y asflags_y ccflags_y cxxflags_y ldsflags_y symflags_y

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
      cmd_cc_i_c = $(CPP) $(c_flags) -o $@ $<
$(obj)/%.i: $(src)/%.c FORCE
	$(call if_changed_dep,cc_i_c)

# C (.c) files
# The C file is compiled and updated dependency information is generated.
# (See cmd_cc_o_c + relevant part of rule_cc_o_c)
quiet_cmd_cc_o_c = $(ECHO_CC) $@
      cmd_cc_o_c = $(CC) $(c_flags) -c -o $@ $<
$(obj)/%.o: $(src)/%.c FORCE
	$(call if_changed_dep,cc_o_c)

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

# Assembly constants (.sym -> .h)
# ---------------------------------------------------------------------------

quiet_cmd_cc_sym_h = $(ECHO_SYM) $@
      cmd_cc_sym_h = $(PYTHON) $(build_home)/tools/gen-as-const.py \
                     --cc "$(CC) $(c_flags)" $< > $@; \
                     if test ! -s $@; then $(RM) $@; false; fi
$(obj)/%.h: $(src)/%.sym FORCE
	$(call if_changed_dep,cc_sym_h)

# Rule to compile a set of .o files into one .a file
quiet_cmd_link_a_target = $(ECHO_AR) $@
      cmd_link_a_target = $(RM) $@; \
                          $(AR) cDPrST $@ $(obj_file) $(obj_subfile)
$(builtin_target): $(obj_file) $(obj_subfile) $(extra_file) FORCE
	$(call if_changed,link_a_target)

$(obj_subfile): $(obj)/%/built-in.a: $(obj)/% ;
