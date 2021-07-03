# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Make bin
# ==========================================================================

#
# Include bin rule
include $(BUILD_HOME)/auxiliary/bin_rule.mk

########################################
# Start build                          #
########################################

quiet_cmd_build_bin = $(ECHO_BIN)  $@
      cmd_build_bin = $(OBJCOPY)                            \
                      $(bin-flags-y)                        \
                      $($@-flags-y)                         \
                      $(addprefix $(obj)/,$($(@F)-obj-y))   \
                      $@
$(bin): FORCE
	$(error_ignored) $(call if_changed,build_bin)
$(call multi_depend, $(bin), , -obj-y)

targets += $(bin)
