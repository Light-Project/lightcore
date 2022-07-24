# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# host rule
# ==========================================================================

# hostprogs-always-y += foo
# ... is a shorthand for
# hostprogs += foo
# always-y  += foo
hostprogs       += $(hostprogs-always-y)
host_always     += $(hostprogs-always-y)

########################################
# sort files                           #
########################################

hostprogs       := $(sort $(hostprogs))
host_cshlib     := $(sort $(hostlibs-y) $(hostlibs-m))
host_cxxshlib   := $(sort $(hostcxxlibs-y) $(hostcxxlibs-m))

########################################
# filter files                         #
########################################

# Executables compiled from a single .c file
host_csingle   := $(foreach m,$(hostprogs), \
                  $(if $($(m)-objs)$($(m)-cxxobjs)$($(m)-sharedobjs),,$(m)))

# C executables linked based on several .o files
host_cmulti    := $(foreach m,$(hostprogs), \
                  $(if $($(m)-cxxobjs),,$(if $($(m)-objs),$(m))))

# Shared object libraries
host_shared    := $(foreach m,$(hostprogs),$(if $($(m)-sharedobjs),$(m)))

# Object (.o) files compiled from .c files
host_cobjs     := $(foreach m,$(hostprogs),$($(m)-objs))

# C++ executables compiled from at least one .cc file and zero or more .c files
host_cxxmulti  := $(foreach m,$(hostprogs),$(if $($(m)-cxxobjs),$(m)))

# C++ Object (.o) files compiled from .cc files
host_cxxobjs   := $(foreach m,$(host-cxxmulti),$($(m)-cxxobjs))

# Object (.o) files used by the shared libaries
host_cshobjs   := $(foreach m,$(host-cshlib),$($(m:.so=-objs)))
host_cxxshobjs := $(foreach m,$(host-cxxshlib),$($(m:.so=-objs)))

########################################
# add path                             #
########################################

host_csingle    := $(strip $(sort $(addprefix $(obj)/,$(host_csingle))))
host_cmulti     := $(strip $(sort $(addprefix $(obj)/,$(host_cmulti))))
host_shared     := $(strip $(sort $(addprefix $(obj)/,$(host_shared))))
host_cobjs      := $(strip $(sort $(addprefix $(obj)/,$(host_cobjs))))
host_cxxmulti   := $(strip $(sort $(addprefix $(obj)/,$(host_cxxmulti))))
host_cxxobjs    := $(strip $(sort $(addprefix $(obj)/,$(host_cxxobjs))))
host_cshlib     := $(strip $(sort $(addprefix $(obj)/,$(host_cshlib))))
host_cxxshlib   := $(strip $(sort $(addprefix $(obj)/,$(host_cxxshlib))))
host_cshobjs    := $(strip $(sort $(addprefix $(obj)/,$(host_cshobjs))))
host_cxxshobjs  := $(strip $(sort $(addprefix $(obj)/,$(host_cxxshobjs))))

host_always     := $(strip $(sort $(addprefix $(obj)/,$(host_always))))

########################################
# targets rule                         #
########################################

host_targets    := $(host_csingle)
host_targets    += $(host_cmulti)
host_targets    += $(host_shared)
host_targets    += $(host_cobjs)
host_targets    += $(host_cxxmulti)
host_targets    += $(host_cxxobjs)
host_targets    += $(host_cshlib)
host_targets    += $(host_cxxshlib)
host_targets    += $(host_cshobjs)
host_targets    += $(host_cxxshobjs)

host_targets    += $(call intermediate_targets, $(host_targets), .lex.o, .lex.c)
host_targets    += $(call intermediate_targets, $(host_targets), .tab.o, .tab.c .tab.h)

always          += $(host_always)
targets         += $(host_targets)
clean-files     += $(host_targets)
