# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# main rule
# ==========================================================================

########################################
# Always rule                          #
########################################

hostprogs       += $(hostprogs-always-y)
host-always     += $(hostprogs-always-y)

always_y        += $(always-y)
always_y        += $(builtin-target) $(extra-file)
always_y        += $(host-targets)

########################################
# Sort files                           #
########################################

subdir-y        := $(obj-y) $(extra-y) $(subdir-y)
subdir-y        := $(strip $(sort $(subdir-y)))
subdir-y        := $(filter %/, $(subdir-y))

dis-subdir-y    := $(obj-) $(extra-)
dis-subdir-y    := $(strip $(sort $(dis-subdir-y)))
dis-subdir-y    := $(filter %/, $(dis-subdir-y))

obj-file        := $(filter-out %/, $(obj-y))

obj-subfile     := $(filter %/, $(obj-y))
obj-subfile     := $(patsubst %/, %/built-in.a, $(obj-subfile))
extra-file      := $(filter-out %/, $(extra-y))

obj-dis         := $(filter-out %/, $(obj-))

builtin-target  := $(obj)built-in.a

hostprogs       := $(sort $(hostprogs))
host-cshlib     := $(sort $(hostlibs-y) $(hostlibs-m))
host-cxxshlib   := $(sort $(hostcxxlibs-y) $(hostcxxlibs-m))

# C code
# Executables compiled from a single .c file
host-csingle	:= $(foreach m,$(hostprogs),$(if $($(m)-objs)$($(m)-cxxobjs)$($(m)-sharedobjs),,$(m)))

# C executables linked based on several .o files
host-cmulti		:= $(foreach m,$(hostprogs),$(if $($(m)-cxxobjs),,$(if $($(m)-objs),$(m))))

# Shared object libraries
host-shared		:= $(foreach m,$(hostprogs),$(if $($(m)-sharedobjs),$(m)))

# Object (.o) files compiled from .c files
host-cobjs		:= $(sort $(foreach m,$(hostprogs),$($(m)-objs)))

# C++ code
# C++ executables compiled from at least one .cc file and zero or more .c files
host-cxxmulti	:= $(foreach m,$(hostprogs),$(if $($(m)-cxxobjs),$(m)))

# C++ Object (.o) files compiled from .cc files
host-cxxobjs	:= $(sort $(foreach m,$(host-cxxmulti),$($(m)-cxxobjs)))

# Object (.o) files used by the shared libaries
host-cshobjs	:= $(sort $(foreach m,$(host-cshlib),$($(m:.so=-objs))))
host-cxxshobjs	:= $(sort $(foreach m,$(host-cxxshlib),$($(m:.so=-objs))))

########################################
# Add path                             #
########################################

subdir-y        := $(addprefix $(obj),$(subdir-y))
dis-subdir-y    := $(addprefix $(obj),$(dis-subdir-y))
obj-dis         := $(addprefix $(obj),$(obj-dis))
obj-file        := $(addprefix $(obj),$(obj-file))
extra-file      := $(addprefix $(obj),$(extra-file))
obj-subfile     := $(addprefix $(obj),$(obj-subfile))

host-csingle    := $(addprefix $(obj),$(host-csingle))
host-cmulti     := $(addprefix $(obj),$(host-cmulti))
host-shared     := $(addprefix $(obj),$(host-shared))
host-cobjs      := $(addprefix $(obj),$(host-cobjs))
host-cxxmulti   := $(addprefix $(obj),$(host-cxxmulti))
host-cxxobjs    := $(addprefix $(obj),$(host-cxxobjs))
host-cshlib     := $(addprefix $(obj),$(host-cshlib))
host-cxxshlib   := $(addprefix $(obj),$(host-cxxshlib))
host-cshobjs    := $(addprefix $(obj),$(host-cshobjs))
host-cxxshobjs  := $(addprefix $(obj),$(host-cxxshobjs))
host-always     := $(addprefix $(obj),$(host-always))

########################################
# targets rule                         #
########################################

main-targets    += $(obj-file)
main-targets    += $(extra-file)
main-targets    += $(builtin-target)
main-targets    += $(lib-target)

host-targets    += $(host-csingle)
host-targets    += $(host-cmulti)
host-targets    += $(host-shared)
host-targets    += $(host-cobjs)
host-targets    += $(host-cxxmulti)
host-targets    += $(host-cxxobjs)
host-targets    += $(host-cshlib)
host-targets    += $(host-cxxshlib)
host-targets    += $(host-cshobjs)
host-targets    += $(host-cxxshobjs)

targets         += $(main-targets) $(host-targets)

########################################
# clean rule                           #
########################################

clean-dire-y    += $(main-targets) $(obj-dis)
clean-dire-y    += $(host-targets)
clean-subdire-y += $(subdir-y) $(dis-subdir-y)
