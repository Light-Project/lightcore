# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# Cleaning up
# ==========================================================================
_clean:

src := $(obj)

clean-y         :=
clean-files     :=
clean-subdir-y  :=
clean-subdirs   :=

# Read auto.conf if it exists, otherwise ignore
-include $(srctree)/include/config/auto.conf

# Include Buildsystem function
include $(build_home)/include/define.mk

# The filename Kbuild has precedence over Makefile
build-dir := $(if $(filter /%,$(src)),$(src),$(srctree)/$(src))
build-file := $(if $(wildcard $(build-dir)/Kbuild),$(build-dir)/Kbuild,$(build-dir)/Makefile)
include $(build-file)

# Include Build rule
include $(build_home)/include/rule.mk

########################################
# Filter files                         #
########################################

clean_file      := $(filter-out %/,$(clean-y))
clean_file      := $(addprefix $(obj)/,$(clean_file)) $(clean-files)
clean_cmd       := $(wildcard $(foreach f,$(clean_file),$(dir $(f)).$(notdir $(f)).cmd))
clean_cmd       += $(wildcard $(foreach f,$(clean_file),$(dir $(f)).$(notdir $(f)).d))

# These files will be cleaned up
clean_files     := $(clean_file) $(clean_cmd)
clean_files     := $(wildcard $(clean_files))

# These folders will be cleaned up
clean_dirs      := $(strip $(sort $(clean-y)))
clean_dirs      := $(filter %/,$(clean_dirs))
clean_dirs      := $(patsubst %/,%,$(clean_dirs))
clean_dirs      := $(addprefix $(obj)/,$(clean_dirs))
clean_dirs      := $(wildcard $(clean_dirs))

# The system recurses these folders down
clean_subdir    := $(clean-subdir-y)
clean_subdir    := $(strip $(sort $(clean_subdir)))
clean_subdir    := $(filter %/,$(clean_subdir))
clean_subdir    := $(patsubst %/,%,$(clean_subdir))
clean_subdir    := $(addprefix $(obj)/,$(clean_subdir))
clean_subdir    += $(clean-subdirs)

# Avoid conflict
clean_files     := $(addprefix _clean_,$(clean_files))
clean_dirs      := $(addprefix _clean_,$(clean_dirs))
clean_subdir    := $(addprefix _clean_,$(clean_subdir))

########################################
# clean rule                           #
########################################

quiet_cmd_clean_file = $(ECHO_CLEAN) $(patsubst _clean_%,%,$@)
      cmd_clean_file = $(RM) -f $(patsubst _clean_%,%,$@)
$(clean_files): FORCE
	$(call cmd,clean_file)

quiet_cmd_clean_dir = $(ECHO_CLEAN_DIR) $(patsubst _clean_%,%,$@)
      cmd_clean_dir = $(RM) -rf $(patsubst _clean_%,%,$@)
$(clean_dirs): FORCE
	$(call cmd,clean_dir)

########################################
# Descending clean                     #
########################################

PHONY += $(clean_subdir)
$(clean_subdir):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

########################################
# Start clean                          #
########################################

PHONY += _clean
_clean: $(clean_subdir) $(clean_files) $(clean_dirs)
	@:

# We are always building only modules.
PHONY += FORCE
FORCE:

# Declare the contents of the PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
