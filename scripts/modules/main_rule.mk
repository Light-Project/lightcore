# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# main rule
# ==========================================================================

obj_file        := $(filter-out %/, $(obj-y))
extra_file      := $(filter-out %/, $(extra-y))

obj_dis         := $(filter-out %/, $(obj-))
extra_dis       := $(filter-out %/, $(extra-))

obj_subfile     := $(filter %/, $(obj-y))
obj_subfile     := $(patsubst %/, %/built-in.a, $(obj_subfile))

subdir_y        := $(obj-y) $(extra-y) $(subdir-y)
subdir_y        := $(filter %/, $(subdir_y))
subdir_y        := $(patsubst %/, %, $(subdir_y))

subdir_n        := $(obj-) $(extra-) $(subdir-)
subdir_n        := $(filter %/, $(subdir_n))
subdir_n        := $(patsubst %/, %, $(subdir_n))

builtin_target  := $(obj)/built-in.a

########################################
# add path                             #
########################################

obj_file        := $(strip $(sort $(addprefix $(obj)/,$(obj_file))))
extra_file      := $(strip $(sort $(addprefix $(obj)/,$(extra_file))))
obj_dis         := $(strip $(sort $(addprefix $(obj)/,$(obj_dis))))
extra_dis       := $(strip $(sort $(addprefix $(obj)/,$(extra_dis))))
obj_subfile     := $(strip $(sort $(addprefix $(obj)/,$(obj_subfile))))
subdir_y        := $(strip $(sort $(addprefix $(obj)/,$(subdir_y))))
subdir_n        := $(strip $(sort $(addprefix $(obj)/,$(subdir_n))))

########################################
# targets rule                         #
########################################

main_targets    := $(obj_file)
main_targets 	+= $(obj_subfile)
main_targets    += $(extra_file)
main_targets    += $(builtin_target)

targets         += $(main_targets)
always          += $(builtin_target)
clean-files     += $(main_targets) $(obj_dis) $(extra_dis)
clean-subdirs   += $(subdir_y) $(subdir_n)
