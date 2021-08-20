# SPDX-License-Identifier: GPL-2.0
# include/asm-generic contains a lot of files that are used
# verbatim by several architectures.
#
# This Makefile reads the file arch/$(SRCARCH)/include/(uapi/)/asm/Kbuild
# and for each file listed in this file with generic-y creates
# a small wrapper file in arch/$(SRCARCH)/include/generated/(uapi/)/asm.

PHONY := all
all:

# $(generic)/Kbuild lists mandatory-y.
include $(generic)Makefile

# Include Buildsystem function
include scripts/include.mk

redundant := $(filter $(mandatory-y) $(generated-y), $(generic-y))
redundant += $(foreach f, $(generic-y), $(if $(wildcard $(obj)$(f)),$(f)))
redundant := $(sort $(redundant))
$(if $(redundant),\
	$(warning redundant generic-y found in $(obj)Kbuild: $(redundant)))

# If arch does not implement mandatory headers, fallback to asm-generic ones.
mandatory-y := $(filter-out $(generated-y), $(mandatory-y))
generic-y   += $(foreach f, $(mandatory-y), $(if $(wildcard $(obj)$(f)),,$(f)))
generic-y   := $(addprefix $(obj), $(generic-y))

quiet_cmd_wrap = WRAP    $@
      cmd_wrap = echo "\#include <asm-generic/$*.h>" > $@
$(obj)%.h:
	$(call cmd,wrap)
all: $(generic-y)

# Create output directory. Skip it if at least one old header exists
# since we know the output directory already exists.
ifeq ($(old-headers),)
$(shell mkdir -p $(obj))
endif

.PHONY: $(PHONY)
