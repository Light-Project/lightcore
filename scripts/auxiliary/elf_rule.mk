# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# elf rule
# ==========================================================================

########################################
# Always build                         #
########################################

# bin-always-y += foo
# ... is a shorthand for
# bin += foo
# always-y  += foo
elf             += $(elf-always-y)
elf_always      += $(elf-always-y)

########################################
# Sort files                           #
########################################

elf             := $(sort $(elf))

########################################
# Add path                             #
########################################

elf             := $(addprefix $(obj),$(elf))
elf_always      := $(addprefix $(obj),$(elf_always))

########################################
# Always rule                          #
########################################

always-y	+= $(elf_always)

########################################
# clean rule                           #
########################################

clean-files += $(elf)
