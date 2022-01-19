# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# bin rule
# ==========================================================================

# bin-always-y += foo
# ... is a shorthand for
# bin += foo
# always-y  += foo

bin         += $(bin-always-y)
bin_always  += $(bin-always-y)
bin_dis     += $(bin-always-)
bin_obj     := $(foreach m,$(dump),$($(m)-obj))

########################################
# Add path                             #
########################################

bin         := $(strip $(sort $(addprefix $(obj)/,$(bin))))
bin_always  := $(strip $(sort $(addprefix $(obj)/,$(bin_always))))
bin_dis     := $(strip $(sort $(addprefix $(obj)/,$(bin_dis))))
bin_obj     := $(strip $(sort $(addprefix $(obj)/,$(bin_obj))))

########################################
# targets rule                         #
########################################

bin_targets := $(bin)
bin_targets += $(bin_obj)

always      += $(bin_always)
targets     += $(bin_targets)
clean-files += $(bin) $(bin_dis)
