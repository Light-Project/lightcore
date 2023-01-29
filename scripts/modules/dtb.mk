# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Build dtb
# ==========================================================================

# Include dtb rule
include $(build_home)/modules/dtb_rule.mk

include_path = include
include_file = include/generated/autoconf.h

dtc_cpp_flags += -Wp,-MMD,$(depfile).pre.tmp -nostdinc -undef -D__DTS__
dtc_cpp_flags += $(addprefix -I,$(include_path))
dtc_cpp_flags += $(addprefix -include,$(include_file))
dtc_tmp = $(subst $(comma),_,$(dot-target).dts.tmp)

DTC_FLAGS += -Wno-interrupt_provider

quiet_cmd_dts_o_dtb = $(ECHO_DTB) $@
      cmd_dts_o_dtb = $(HOSTCC) -E $(dtc_cpp_flags) -x assembler-with-cpp -o $(dtc_tmp) $<;   \
                      $(DTC) -o $@ -b 0 $(addprefix -i ,$(dir $<)$(DTC_INCLUDE)) $(DTC_FLAGS) \
                      -d $(depfile).dtc.tmp $(dtc_tmp);                                       \
                      cat $(depfile).pre.tmp $(depfile).dtc.tmp > $(depfile)
$(obj)/%.dtb: $(src)/%.dts FORCE
	$(call if_changed_dep,dts_o_dtb)
