# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Make bin
# ==========================================================================

# Include bin rule
include $(build_home)/modules/bin_rule.mk

bin-flags-y ?= -O binary

quiet_cmd_build_bin = $(ECHO_BIN) $@
      cmd_build_bin = $(OBJCOPY)                            \
                      $(bin-flags-y)                        \
                      $($(@F)-flags-y)                      \
                      $(addprefix $(obj)/,$($(@F)-obj))     \
                      $@
$(bin): FORCE
	$(call if_changed,build_bin)
$(call multi_depend, $(bin), , -obj)
