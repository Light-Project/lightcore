# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# Recursion build system
# ==========================================================================
submk := $(obj)

_build:

# Include Buildsystem function
include $(build_home)/include/define.mk

# Read auto.conf if it exists, otherwise ignore
-include $(srctree)/include/config/auto.conf

# Include sub makefile
sub_dir  := $(if $(filter /%,$(submk)),$(submk),$(srctree)/$(submk))
sub_file := $(if $(wildcard $(sub_dir)/Kbuild),$(sub_dir)/Kbuild,$(sub_dir)/Makefile)
include $(sub-file)

#####################################
# Start project                     #
#####################################

project_y   := $(project-y)
project_y   := $(strip $(sort $(project_y)))
project_y   := $(filter %/, $(project_y))
project_y   := $(patsubst %/,%,$(project_y))
project_y   := $(addprefix $(obj)/,$(project_y))

project_n   := $(project-)
project_n   := $(strip $(sort $(project_n)))
project_n   := $(filter %/, $(project_n))
project_n   := $(patsubst %/,%,$(project_n))
project_n   := $(addprefix $(obj)/,$(project_n))

#####################################
# Start build                       #
#####################################

PHONY += _build
_build: $(project_y)
	$(Q)$(MAKE) $(basic)
	$(Q)$(MAKE) $(build)=$(sub_dir)
	$(call hook_build)

#####################################
# Clean rule                        #
#####################################

PHONY += $(clean-dirs) clean
_clean: $(project_y) $(project_n)
	$(Q)$(MAKE) $(clean)=$(sub_dir)
	$(call hook_clean)

#####################################
# Descending operation              #
#####################################

PHONY += $(project_y)
$(project_y): FORCE
	$(Q)$(MAKE) $(submake)=$@ $(MAKECMDGOALS)

PHONY += $(project_n)
$(project_n): FORCE
	$(Q)$(MAKE) $(submake)=$@ $(MAKECMDGOALS)

# We are always building only modules.
PHONY += FORCE
FORCE:

# Declare the contents of the PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
