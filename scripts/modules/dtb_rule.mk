# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# dtb rule
# ==========================================================================

dtb_file    += $(dtb-y)
dtb_always  += $(dtb-y)

########################################
# Add path                             #
########################################

dtb_file    := $(strip $(sort $(addprefix $(obj)/,$(dtb_file))))
dtb_always  := $(strip $(sort $(addprefix $(obj)/,$(dtb_always))))

########################################
# targets rule                         #
########################################

dtb_targets	+= $(dtb_file)

always      += $(dtb_always)
targets     += $(dtb_targets)
clean-files += $(dtb_targets)
