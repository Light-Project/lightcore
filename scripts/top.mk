# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# System top
# ==========================================================================

_start:

srctree := .
build_home := $(srctree)/scripts

# Read auto.conf if it exists, otherwise ignore
-include include/config/auto.conf

include $(build_home)/include/define.mk
include $(build_home)/include/warn.mk

# Do not use make's built-in rules and variables
# Do not print "Entering directory ...",
MAKEFLAGS   += -rR
MAKEFLAGS   += --no-print-directory

ifeq ("$(origin V)", "command line")
BUILD_VERBOSE = $(V)
else
BUILD_VERBOSE = 0
endif

ifeq ($(BUILD_VERBOSE),1)
quiet       =
Q           =
else
MAKEFLAGS   += -s
quiet       = quiet_
Q           = @
endif

export quiet Q BUILD_VERBOSE

ifeq ($(CONFIG_KERNEL_DEBUG),y)
export G = 1
endif

ifndef Kconfig
export Kconfig := Kconfig
endif

# OK, Make called in directory where kernel src resides
# Do we want to locate output files in a separate directory?
ifeq ("$(origin O)", "command line")
BUILD_CHDIR := $(O)
endif

ifeq ("$(origin M)", "command line")
BUILD_EXTMOD := $(M)
endif

ifneq ($(BUILD_CHDIR),)
abs_objtree := $(shell $(MKDIR) $(BUILD_CHDIR) && cd $(BUILD_CHDIR) && pwd)
$(if $(abs_objtree),,$(error failed to create output directory "$(abs_objtree)"))
else
abs_objtree := $(CURDIR)
endif

ifeq ($(abs_srctree),$(abobjtree))

endif

srctree := .
objtree := .
build_home := $(srctree)/scripts
export srctree objtree build_home

VERSION = test
EXTRAVERSION = -rc0
KERNELVERSION = $(VERSION)$(EXTRAVERSION)
export VERSION EXTRAVERSION KERNELVERSION BUILD_EXTMOD

# ===========================================================================
# Rules shared between *config targets and build targets

# Basic helpers built in scripts/basic/
scripts_basic: FORCE
	$(Q)$(MAKE) $(build)=scripts/basic

# Basic helpers built in scripts/basic/
scripts_dtc: scripts_basic FORCE
	$(Q)$(MAKE) $(build)=scripts/dtc

# Before starting out-of-tree build, make sure the source tree is clean.
# outputmakefile generates a Makefile in the output directory, if using a
# separate output directory. This allows convenient use of make in the
# output directory.
# At the same time when output Makefile generated, generate .gitignore to
# ignore whole output directory
outputmakefile:
ifdef building_out_of_srctree
	$(Q)if [ -f $(srctree)/.config -o \
		 -d $(srctree)/include/config -o \
		 -d $(srctree)/arch/$(SRCARCH)/include/generated ]; then \
		echo >&2 "***"; \
		echo >&2 "*** The source tree is not clean, please run 'make$(if $(findstring command line, $(origin ARCH)), ARCH=$(ARCH)) mrproper'"; \
		echo >&2 "*** in $(abs_srctree)";\
		echo >&2 "***"; \
		false; \
	fi
	$(Q)$(LN) $(srctree) source
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile $(srctree)
	$(Q)test -e .gitignore || \
	{ echo "# this is build directory, ignore it"; echo "*"; } > .gitignore
endif

#####################################
# kconfig rule                      #
#####################################

# ===========================================================================
# *config targets only - make sure prerequisites are updated, and descend
# in scripts/kconfig to make the *config target

# Read arch specific Makefile to set KBUILD_DEFCONFIG as needed.
# KBUILD_DEFCONFIG may point out an alternative default configuration
# used for 'make defconfig'
export KBUILD_DEFCONFIG KBUILD_KCONFIG CC_VERSION_TEXT

config: outputmakefile scripts_basic FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@
	$(Q)$(MAKE) $(build)=scripts/kconfig syncconfig

%config: outputmakefile scripts_basic FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@
	$(Q)$(MAKE) $(build)=scripts/kconfig syncconfig

check_file := .config

ifeq ($(wildcard $(check_file)),)
_start: FORCE
	$(Q)$(ECHO) '##########################################################'
	$(Q)$(ECHO) 'Configuration file ".config" not found!                   '
	$(Q)$(ECHO) '                                                          '
	$(Q)$(ECHO) 'Please run some configurator (e.g. "make oldconfig" or    '
	$(Q)$(ECHO) 'make menuconfig or make xconfig).                         '
	$(Q)$(ECHO) '##########################################################'
else
_start: start FORCE
endif

build/%.o build/%.h build/%.lds: scripts_basic
	$(Q)$(MAKE) $(build)=$(patsubst build/%/,%,$(dir $@)) $(patsubst build/%,%,$@)

build/%: scripts_basic
	$(Q)$(MAKE) $(build)=$(patsubst build/%,%,$@)

#####################################
# Clean rule                        #
#####################################

clean/%: FORCE
	$(Q)$(MAKE) $(clean)=$(patsubst clean/%,%,$@)

clean: FORCE
	$(Q)$(MAKE) $(clean)=$(objtree)

MRPROPER_DIRS   := include/config include/generated
MRPROPER_FILES  := .config .config.old tags TAGS cscope* GPATH GTAGS GRTAGS GSYMS

rm-dirs         := $(wildcard $(MRPROPER_DIRS))
rm-files        := $(wildcard $(MRPROPER_FILES))
mrproper-dirs   := $(addprefix _mrproper_,$(rm-dirs))
mrproper-files  := $(addprefix _mrproper_,$(rm-files))

PHONY += $(mrproper-dirs)
$(mrproper-dirs):
	$(Q)$(ECHO) "$(ECHO_RMDIR) $(patsubst _mrproper_%,%,$@)"
	$(Q)$(RMDIR) $(patsubst _mrproper_%,%,$@)

PHONY += $(mrproper-files)
$(mrproper-files):
	$(Q)$(ECHO) "$(ECHO_RM) $(patsubst _mrproper_%,%,$@)"
	$(Q)$(RM) $(patsubst _mrproper_%,%,$@)

PHONY += mrproper
mrproper: clean $(mrproper-files) $(mrproper-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)

distclean: mrproper FORCE
	$(Q)$(MAKE) $(clean)=$(build_home)
	$(Q)$(MAKE) $(clean)=tools

#####################################
# Help rule                         #
#####################################

help: FORCE
	$(Q)$(ECHO) 'Generic targets:'
	$(Q)$(ECHO) '  all              - Build all targets marked with [*]'
	$(Q)$(ECHO) '  disk             - Build kboot preload and generate a disk image'
	$(Q)$(ECHO) '  uboot            - Build kboot and generate a uboot image'
	$(Q)$(ECHO) '  preload          - Build the preload section only'
	$(Q)$(ECHO) '  kboot            - Build kboot and kernel'
	$(Q)$(ECHO) ''
	$(Q)$(ECHO) 'Other generic targets:'
	$(Q)$(ECHO) '  build/dir        - Build all targets in dir and below'
	$(Q)$(ECHO) '  build/dir/file.* - Build one target in dir'
	$(Q)$(ECHO) '  dtbs             - Build all DTB'
	$(Q)$(ECHO) '  file.dtb         - Build one specific DTB'
	$(Q)$(ECHO) '  clean/dir        - Clean all targets in dir and below'
	$(Q)$(ECHO) ''
	$(Q)$(ECHO) 'Cleaning targets:'
	$(Q)$(ECHO) '  clean            - Remove most generated files but keep the config'
	$(Q)$(ECHO) '  clean/dir        - Build one files in dir'
	$(Q)$(ECHO) '  mrproper         - Remove all generated files + config + various backup files'
	$(Q)$(ECHO) '  distclean        - mrproper + remove editor backup and patch files'
	$(Q)$(ECHO) ''
	$(Q)$(ECHO) '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	$(Q)$(ECHO) '  make V=2   [targets] 2 => give reason for rebuild of target'
	$(Q)$(ECHO) '  make W=n   [targets] Enable extra build checks, n=1,2,3 where'
	$(Q)$(ECHO) '               1: warnings which may be relevant and do not occur too often'
	$(Q)$(ECHO) '               2: warnings which occur quite often but may still be relevant'
	$(Q)$(ECHO) '               3: more obscure warnings, can most likely be ignored'
	$(Q)$(ECHO) '               Multiple levels can be combined with W=12 or W=123'
	$(Q)$(ECHO) ''
	$(Q)$(ECHO) 'Configuration targets:'
	$(Q)$(MAKE) -f scripts/kconfig/Makefile help
	$(Q)$(ECHO) ''
	$(Q)$(ECHO) 'Other targets:'
	$(Q)$(ECHO) '  install          - Install the kernel to the specified path (INSTALL_PATH)'
	$(Q)$(ECHO) '  run              - Execute (download or simulation) the selected platform'

# We are always building only modules.
PHONY += FORCE
FORCE:

# Declare the contents of the PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
