# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# Make asm-generic
# ==========================================================================

all: FORCE

src := $(subst /generated,,$(obj))

# $(generic)/Kbuild lists mandatory-y.
include $(generic)/Makefile

# Include Buildsystem function
include $(build_home)/include/define.mk

redundant := $(filter $(mandatory-y) $(generated-y), $(generic-y))
redundant += $(foreach f, $(generic-y), $(if $(wildcard $(obj)/$(f)),$(f)))
redundant := $(sort $(redundant))
$(if $(redundant), $(warning redundant generic-y found in $(obj)/Kbuild: $(redundant)))

# If arch does not implement mandatory headers, fallback to asm-generic ones.
mandatory-y := $(filter-out $(generated-y), $(mandatory-y))
generic-y   += $(foreach f, $(mandatory-y), $(if $(wildcard $(srctree)/$(src)/$(f)),,$(f)))

generic-y   := $(addprefix $(obj)/, $(generic-y))
generated-y := $(addprefix $(obj)/, $(generated-y))

unwanted    := $(filter-out $(generic-y) $(generated-y),$(old-headers))

path        := $(patsubst include/%,%,$(generic))

########################################
# Start Make                           #
########################################

quiet_cmd_wrap = $(ECHO_WRAP) $@
      cmd_wrap = echo "\#include <$(path)/$*.h>" > $@
$(obj)/%.h:
	$(call cmd,wrap)

quiet_cmd_remove = $(ECHO_RM) $(unwanted)
      cmd_remove = rm -f $(unwanted)

all: $(generic-y)
	$(if $(unwanted),$(call cmd,remove))
	@:

# Create output directory. Skip it if at least one old header exists
# since we know the output directory already exists.
ifeq ($(old-headers),)
$(shell mkdir -p $(obj))
endif

# We are always building only modules.
PHONY += FORCE
FORCE:

.PHONY: $(PHONY)
