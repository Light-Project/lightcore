# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# main rule
# ==========================================================================

subdir-y		:= $(obj-y) $(extra-y) $(subdir-y)
subdir-y		:= $(strip $(sort $(subdir-y)))
subdir-y		:= $(filter %/, $(subdir-y))
subdir-y		:= $(patsubst %/,%,$(subdir-y))

dis-subdir-y 	:= $(obj-) $(extra-)
dis-subdir-y	:= $(strip $(sort $(dis-subdir-y)))
dis-subdir-y	:= $(filter %/, $(dis-subdir-y))
dis-subdir-y	:= $(patsubst %/,%,$(dis-subdir-y))

obj-file	:= $(filter-out %/, $(obj-y))
extra-file	:= $(filter-out %/, $(extra-y))

obj-subfile	:= $(filter %/, $(obj-y))
obj-subfile	:= $(patsubst %/, %/built-in.o, $(obj-subfile))
				
obj-dis		:= $(filter-out %/, $(obj-))

builtin-target := $(obj)/built-in.o

########################################
# Add path                             #
########################################

subdir-y		:= $(addprefix $(obj)/,$(subdir-y))
dis-subdir-y	:= $(addprefix $(obj)/,$(dis-subdir-y))
obj-dis			:= $(addprefix $(obj)/,$(obj-dis))
obj-file		:= $(addprefix $(obj)/,$(obj-file))
extra-file		:= $(addprefix $(obj)/,$(extra-file))
obj-subfile		:= $(addprefix $(obj)/,$(obj-subfile))

# obj-subfile		:= $(wildcard $(obj-subfile))

########################################
# targets rule                         #
########################################

main-targets	+= $(obj-file)
main-targets 	+= $(extra-file)
main-targets	+= $(builtin-target)
main-targets 	+= $(lib-target)
targets			+= $(main-targets)

########################################
# Always rule                          #
########################################

always-y	+= $(builtin-target) $(extra-file)

########################################
# clean rule                           #
########################################

clean-files += $(main-targets) $(obj-dis)
clean-subdir += $(subdir-y) $(dis-subdir-y)
