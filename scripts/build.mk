# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# Build sub system
# ==========================================================================

src := $(obj)

_build:

# Read auto.conf if it exists, otherwise ignore
sinclude $(srctree)/include/config/auto.conf

# Include Buildsystem function
include $(build_home)/include/define.mk

# Include obj makefile
build-dir := $(if $(src), $(if $(filter /%,$(src)),$(src),$(srctree)/$(src)), $(srctree))
build-file := $(if $(wildcard $(build-dir)/Kbuild),$(build-dir)/Kbuild,$(build-dir)/Makefile)
include $(build-file)

#####################################
# Include                           #
#####################################

targets     := $(addprefix $(obj)/,$(targets))
always      := $(addprefix $(obj)/,$(always-y))

include_y   := $(include-y)
include_y   := $(strip $(sort $(include_y)))
include_y   := $(addprefix $(obj)/,$(include_y))
unexport include-y include-direct-y

include_blend += $(include_y) $(include-direct-y)
export include_blend

#####################################
# Always build                      #
#####################################

# hostprogs-always-y += foo
# ... is a shorthand for
# hostprogs += foo

hostprogs   += $(hostprogs-always-y)
elf         += $(elf-always-y)
bin         += $(bin-always-y)
dump        += $(dump-always-y)

#####################################
# basic rule                        #
#####################################

include $(build_home)/modules/pack.mk

ifneq ($(bin),)
include $(build_home)/modules/bin.mk
endif

ifneq ($(dtb-y),)
include $(build_home)/modules/dtb.mk
endif

ifneq ($(dump),)
include $(build_home)/modules/dump.mk
endif

ifneq ($(elf),)
include $(build_home)/modules/elf.mk
endif

ifneq ($(hostprogs),)
include $(build_home)/modules/host.mk
endif

include $(build_home)/modules/main.mk

#####################################
# Descending build                  #
#####################################

PHONY += $(subdir_y)
$(subdir_y):
	$(Q)$(MAKE) $(build)=$@

#####################################
# Start build                       #
#####################################

_build: $(subdir_y) $(always) FORCE
	@:

# Add FORCE to the prequisites of a target to force it to be always rebuilt.
# ---------------------------------------------------------------------------
PHONY += FORCE

FORCE:

# Read all saved command lines and dependencies for the $(targets) we
# may be building above, using $(if_changed{,_dep}). As an
# optimization, we don't need to read them if the target does not
# exist, we will rebuild anyway in that case.

targets     := $(wildcard $(sort $(targets)))
cmd_files   := $(wildcard $(foreach f,$(targets),$(dir $(f)).$(notdir $(f)).cmd))

-include $(cmd_files)

# Create directories for object files if they do not exist
obj-dirs := $(sort $(patsubst %/,%, $(dir $(targets))))

# If targets exist, their directories apparently exist. Skip mkdir.
existing-dirs := $(sort $(patsubst %/,%, $(dir $(existing-targets))))
obj-dirs := $(strip $(filter-out $(existing-dirs), $(obj-dirs)))

ifneq ($(obj-dirs),)
$(shell mkdir -p $(obj-dirs))
endif

.PHONY: $(PHONY)
