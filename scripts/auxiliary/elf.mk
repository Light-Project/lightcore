# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Build elf
# ==========================================================================

#
# Include bin rule
include $(BUILD_HOME)/auxiliary/elf_rule.mk

########################################
# Start build                          #
########################################

# Create executable from build-in .o file
# host-csingle -> Executable
quiet_cmd_build_elf = $(ECHO_ELF)  $@
      cmd_build_elf = $(MKELF) $($(@F)-flags-y) -o $@    \
                      $(addprefix $(obj)/,$($(@F)-obj-y))\
	                  $($(@F)-direct-y)
$(elf): FORCE
	$(error_ignored) $(call if_changed,build_elf)
$(call multi_depend, $(elf), , -obj-y)

targets += $(elf)
