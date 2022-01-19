# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# elf rule
# ==========================================================================

# elf-always-y += foo
# ... is a shorthand for
# elf += foo
# always-y  += foo

elf             += $(elf-always-y)
elf_always      += $(elf-always-y)
elf_dis         += $(elf-always-)
elf_objs     	:= $(foreach m,$(elf),$($(m)-objs))

########################################
# Add path                             #
########################################

elf_file        := $(strip $(sort $(addprefix $(obj)/,$(elf))))
elf_always      := $(strip $(sort $(addprefix $(obj)/,$(elf_always))))
elf_dis         := $(strip $(sort $(addprefix $(obj)/,$(elf_dis))))
elf_objs        := $(strip $(sort $(addprefix $(obj)/,$(elf_objs))))

########################################
# Always rule                          #
########################################

elf_targets     := $(elf_file)
elf_targets     += $(elf_objs)

always          += $(elf_always)
targets         += $(elf_targets)
clean-files     += $(elf_targets)
clean-files     += $(wildcard $(foreach f,$(elf_file),$(f).map))
