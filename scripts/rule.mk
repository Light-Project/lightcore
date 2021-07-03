# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# main rule
# ==========================================================================

subdir-y        := $(obj-y) $(extra-y) $(subdir-y)
subdir-y        := $(strip $(sort $(subdir-y)))
subdir-y        := $(filter %/, $(subdir-y))
subdir-y        := $(patsubst %/,%,$(subdir-y))

dis-subdir-y    := $(obj-) $(extra-)
dis-subdir-y    := $(strip $(sort $(dis-subdir-y)))
dis-subdir-y    := $(filter %/, $(dis-subdir-y))
dis-subdir-y    := $(patsubst %/,%,$(dis-subdir-y))

obj-file        := $(filter-out %/, $(obj-y))
extra-file      := $(filter-out %/, $(extra-y))

obj-subfile     := $(filter %/, $(obj-y))
obj-subfile     := $(patsubst %/, %/built-in.a, $(obj-subfile))

obj-dis         := $(filter-out %/, $(obj-))

builtin-target  := $(obj)/built-in.a

########################################
# Add path                             #
########################################

subdir-y        := $(addprefix $(obj)/,$(subdir-y))
dis-subdir-y    := $(addprefix $(obj)/,$(dis-subdir-y))
obj-dis         := $(addprefix $(obj)/,$(obj-dis))
obj-file        := $(addprefix $(obj)/,$(obj-file))
extra-file      := $(addprefix $(obj)/,$(extra-file))
obj-subfile     := $(addprefix $(obj)/,$(obj-subfile))

########################################
# targets rule                         #
########################################

main-targets    += $(obj-file)
main-targets    += $(extra-file)
main-targets    += $(builtin-target)
main-targets    += $(lib-target)
targets         += $(main-targets)

########################################
# Always rule                          #
########################################

always_y        += $(always-y)
always_y        += $(builtin-target) $(extra-file)

########################################
# clean rule                           #
########################################

clean-files += $(main-targets) $(obj-dis)
clean-subdir += $(subdir-y) $(dis-subdir-y)



########################################
# Always build                         #
########################################

# hostprogs-always-y += foo
# ... is a shorthand for
# hostprogs += foo
# always-y  += foo
hostprogs       += $(hostprogs-always-y)
host-always     += $(hostprogs-always-y)

########################################
# Sort files                           #
########################################

hostprogs       := $(sort $(hostprogs))
host-cshlib     := $(sort $(hostlibs-y) $(hostlibs-m))
host-cxxshlib   := $(sort $(hostcxxlibs-y) $(hostcxxlibs-m))

########################################
# Filter files                         #
########################################

# C code
# Executables compiled from a single .c file
host-csingle	:= $(foreach m,$(hostprogs), \
			        $(if $($(m)-objs)$($(m)-cxxobjs)$($(m)-sharedobjs),,$(m)))

# C executables linked based on several .o files
host-cmulti	:= $(foreach m,$(hostprogs), \
		       $(if $($(m)-cxxobjs),,$(if $($(m)-objs),$(m))))

# Shared object libraries
host-shared	:= $(foreach m,$(hostprogs),\
		   	   $(if $($(m)-sharedobjs),$(m)))

# Object (.o) files compiled from .c files
host-cobjs	:= $(sort $(foreach m,$(hostprogs),$($(m)-objs)))

# C++ code
# C++ executables compiled from at least one .cc file
# and zero or more .c files
host-cxxmulti	:= $(foreach m,$(hostprogs),$(if $($(m)-cxxobjs),$(m)))

# C++ Object (.o) files compiled from .cc files
host-cxxobjs	:= $(sort $(foreach m,$(host-cxxmulti),$($(m)-cxxobjs)))

# Object (.o) files used by the shared libaries
host-cshobjs	:= $(sort $(foreach m,$(host-cshlib),$($(m:.so=-objs))))
host-cxxshobjs	:= $(sort $(foreach m,$(host-cxxshlib),$($(m:.so=-objs))))

########################################
# Add path                             #
########################################

host-csingle    := $(addprefix $(obj)/,$(host-csingle))
host-cmulti     := $(addprefix $(obj)/,$(host-cmulti))
host-shared     := $(addprefix $(obj)/,$(host-shared))
host-cobjs      := $(addprefix $(obj)/,$(host-cobjs))
host-cxxmulti   := $(addprefix $(obj)/,$(host-cxxmulti))
host-cxxobjs    := $(addprefix $(obj)/,$(host-cxxobjs))
host-cshlib     := $(addprefix $(obj)/,$(host-cshlib))
host-cxxshlib   := $(addprefix $(obj)/,$(host-cxxshlib))
host-cshobjs    := $(addprefix $(obj)/,$(host-cshobjs))
host-cxxshobjs  := $(addprefix $(obj)/,$(host-cxxshobjs))
host-always     := $(addprefix $(obj)/,$(host-always))

########################################
# targets rule                         #
########################################

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
targets         += $(host-targets)

########################################
# Always rule                          #
########################################

always_y        += $(host-targets)

########################################
# clean rule                           #
########################################

clean-files += $(host-targets)
