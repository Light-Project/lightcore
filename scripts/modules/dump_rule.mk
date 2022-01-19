
# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# dump rule
# ==========================================================================

# bin-always-y += foo
# ... is a shorthand for
# bin += foo
# always-y  += foo

dump            += $(dump-always-y)
dump_always     += $(dump-always-y)
dump_dis        += $(dump-always-)
dump_obj        := $(foreach m,$(dump),$($(m)-obj))

########################################
# Add path                             #
########################################

dump            := $(strip $(sort $(addprefix $(obj)/,$(dump))))
dump_always     := $(strip $(sort $(addprefix $(obj)/,$(dump_always))))
dump_dis        := $(strip $(sort $(addprefix $(obj)/,$(dump_dis))))
dump_obj        := $(strip $(sort $(addprefix $(obj)/,$(dump_obj))))

########################################
# targets rule                         #
########################################

dump_targets    := $(dump)
dump_targets    += $(dump_obj)

always          += $(dump_always)
targets         += $(dump_targets)
clean-files     += $(dump) $(dump_dis)
