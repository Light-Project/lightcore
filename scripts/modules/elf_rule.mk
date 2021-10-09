# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# elf rule
# ==========================================================================

# bin-always-y += foo
# ... is a shorthand for
# bin += foo
# always-y  += foo
elf             += $(elf-always-y)
elf_always      += $(elf-always-y)

elf_objs     	:= $(foreach m,$(elf),$($(m)-objs))

########################################
# Add path                             #
########################################

elf_file        := $(strip $(sort $(addprefix $(obj)/,$(elf))))
elf_objs        := $(strip $(sort $(addprefix $(obj)/,$(elf_objs))))
elf_always      := $(strip $(sort $(addprefix $(obj)/,$(elf_always))))

########################################
# Always rule                          #
########################################

elf_targets     := $(elf_file)
elf_targets     += $(elf_objs)

always          += $(elf_always)
targets         += $(elf_targets)
clean-files     += $(elf_targets)
clean-files     += $(wildcard $(foreach f,$(elf_file),$(f).map))
