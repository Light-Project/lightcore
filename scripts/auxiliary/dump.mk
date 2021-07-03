# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Make dump
# ==========================================================================

#
# Include bin rule
include $(BUILD_HOME)/auxiliary/dump_rule.mk

########################################
# Start build                          #
########################################

# Create executable from a single .c file
# host-csingle -> Executable
quiet_cmd_build_dump = $(ECHO_DUMP)  $@
      cmd_build_dump = $(OBJDUMP)                           \
                       $(dump-flags-y)                      \
                       $($(@F)-flags-y)                     \
                       $(addprefix $(obj)/,$($(@F)-obj-y))  \
                       > $@
$(dump): FORCE
	$(error_ignored) $(call if_changed,build_dump)
$(call multi_depend, $(dump), , -obj-y)

targets += $(dump)