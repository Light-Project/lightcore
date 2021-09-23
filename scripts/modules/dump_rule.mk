
# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# dump rule
# ==========================================================================

# bin-always-y += foo
# ... is a shorthand for
# bin += foo
# always-y  += foo
dump        += $(dump-always-y)
dump-always += $(dump-always-y)
dump-dis    += $(dump-always-)

########################################
# Add path                             #
########################################

dump        := $(strip $(sort $(addprefix $(obj),$(dump))))
dump-always := $(strip $(sort $(addprefix $(obj),$(dump-always))))
dump-dis    := $(strip $(sort $(addprefix $(obj),$(dump-dis))))

########################################
# targets rule                         #
########################################

always-y    += $(dump-always)
clean-files += $(dump) $(dump-dis)
