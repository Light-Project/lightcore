# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Build elf
# ==========================================================================

# Include bin rule
include $(build_home)/modules/elf_rule.mk

quiet_cmd_link_o_target = $(ECHO_LD) $@
      cmd_link_o_target = $(RANLIB) $<; \
                          $(LD) $(ldflags-y) -r -o $@ --whole-archive $<
$(elf_objs): $(obj)/%.o: $(src)/%.a FORCE
	$(call if_changed,link_o_target)

quiet_cmd_build_elf = $(ECHO_ELF) $@
      cmd_build_elf = $(MKELF) -o $@ $(addprefix $(obj)/,$($(@F)-objs)) \
                      $(elfflags-y) $($(@F)-flags-y)
$(elf_file): FORCE
	$(call if_changed,build_elf)
$(call multi_depend, $(elf_file), , -objs)
