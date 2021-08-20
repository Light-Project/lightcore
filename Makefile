# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# System top
# ==========================================================================

VERSION = test
EXTRAVERSION = -rc0
KERNELVERSION = $(VERSION)$(EXTRAVERSION)

export VERSION EXTRAVERSION KERNELVERSION

PHONY += __all
__all:

# Do not use make's built-in rules and variables
# Do not print "Entering directory ...",
MAKEFLAGS   += -rR
MAKEFLAGS   += --no-print-directory

# Beautify output
# ---------------------------------------------------------------------------
#
# Normally, we echo the whole command before executing it. By making
# that echo $($(quiet)$(cmd)), we now have the possibility to set
# $(quiet) to choose other forms of output instead, e.g.
#
#         quiet_cmd_cc_o_c = Compiling $(RELDIR)/$@
#         cmd_cc_o_c       = $(CC) $(c_flags) -c -o $@ $<
#
# If $(quiet) is empty, the whole command will be printed.
# If it is set to "quiet_", only the short version will be printed.
# If it is set to "silent_", nothing will be printed at all, since
# the variable $(silent_cmd_cc_o_c) doesn't exist.
#
# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
# If KBUILD_VERBOSE equals 0 then the above command will be hidden.
# If KBUILD_VERBOSE equals 1 then the above command is displayed.
# If KBUILD_VERBOSE equals 2 then give the reason why each target is rebuilt.
#
# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands

ifndef V
DEBUG_MODE  := 0
endif

ifeq ("$(origin V)", "command line")
DEBUG_MODE  = $(V)
endif

ifeq ($(DEBUG_MODE),0)
quiet       = quiet_
MAKEFLAGS   += -s
Q           = @
endif
ifeq ($(DEBUG_MODE),1)
quiet       =
Q           =
endif
export quiet Q

########################################
# Start path                           #
########################################

# Kconfig path config
ifndef Kconfig
Kconfig := Kconfig
endif

export Kconfig

# Read auto.conf if it exists, otherwise ignore
-include include/config/auto.conf

include platform.mk
export CROSS_COMPILE ARCH

include scripts/include.mk

# ===========================================================================
# Rules shared between *config targets and build targets

# Basic helpers built in scripts/basic/
PHONY += scripts_basic
scripts_basic:
	$(Q)$(MAKE) $(build)=scripts/basic/
	$(Q)rm -f .tmp_quiet_recordmcount

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
	$(Q)ln -fsn $(srctree) source
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile $(srctree)
	$(Q)test -e .gitignore || \
	{ echo "# this is build directory, ignore it"; echo "*"; } > .gitignore
endif

# ===========================================================================
# *config targets only - make sure prerequisites are updated, and descend
# in scripts/kconfig to make the *config target

# Read arch specific Makefile to set KBUILD_DEFCONFIG as needed.
# KBUILD_DEFCONFIG may point out an alternative default configuration
# used for 'make defconfig'
export KBUILD_DEFCONFIG KBUILD_KCONFIG CC_VERSION_TEXT

config: outputmakefile scripts_basic FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig/ $@
	$(Q)$(MAKE) $(build)=scripts/kconfig/ syncconfig

%config: outputmakefile scripts_basic FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig/ $@
	$(Q)$(MAKE) $(build)=scripts/kconfig/ syncconfig

%doc:
	$(Q)$(MAKE) $(build)=doc/ $@

usr-%: FORCE
	$(Q)$(MAKE) $(build)=usr/ $@

#####################
# Subproject
#####################

kernel-y += arch/
kernel-y += crypto/
kernel-y += doc/
kernel-y += drivers/
kernel-y += fs/
kernel-y += init/
kernel-y += ipc/
kernel-y += kernel/
kernel-y += lib/
kernel-y += mm/
kernel-y += net/
kernel-y += virt/

#####################
# Global Config
#####################

ifeq ($(CONFIG_KERNEL_DEBUG),y)
G = 1
endif
export G
export BUILD_ENABLE_EXTRA_GCC_DEBUG := $(G)

include-y   += include/kconfig.h                       \
               include/compiler/compiler_attributes.h  \
               include/compiler/compiler_type.h        \
               include/compiler/compiler.h             \
               include/compiler/sections.h             \
               include/compiler/stringify.h
include-y   += include/
include-y   += arch/$(arch)/include/ \
               arch/$(arch)/include/generated/

# asflags
sys-asflags-y += -D__ASSEMBLY__

# ccflags
sys-ccflags-y += -std=gnu11

# acflags
sys-acflags-$(CONFIG_CC_OPTIMIZE_FOR_DEBUG)         += -O0
sys-acflags-$(CONFIG_CC_OPTIMIZE_FOR_PERFORMANCE)   += -O2
sys-acflags-$(CONFIG_CC_OPTIMIZE_FOR_SIZE)          += -Os
sys-acflags-$(CONFIG_STRICT) += -Wall -Werror
sys-acflags-y += -ffreestanding -nostdinc -fno-builtin -static
sys-acflags-y += -fno-pic -fno-PIE
sys-acflags-y += -fno-stack-protector

# ldsflags
sys-ldsflags-y += -D__ASSEMBLY__

asflags-y   := $(strip $(sys-asflags-y) $(platform-asflags-y))
ccflags-y   := $(strip $(sys-ccflags-y) $(platform-ccflags-y))
acflags-y   := $(strip $(sys-acflags-y) $(platform-acflags-y))
ldflags-y   := $(strip $(sys-ldflags-y) $(platform-ldflags-y))
ldsflags-y  := $(strip $(sys-ldsflags-y) $(platform-ldsflags-y))

ifdef CONFIG_LD_DEAD_CODE_DATA_ELIMINATION
acflags-y += -ffunction-sections -fdata-sections
kernel-flags-y += -Wl,--gc-sections
endif

kernel-flags-$(CONFIG_KERNEL_MAP) += -Wl,--cref,-Map=$@.map
kernel-flags-y += -e boot_head $(platform-eflags-y)
kernel-flags-y += -Wl,--build-id=sha1

export CROSS_COMPILE include-y asflags-y ccflags-y acflags-y ldsflags-y ldflags-y

#############################################################
# Compiler instruction
#############################################################

check_file := .config include/generated/autoconf.h

ifneq ($(wildcard $(check_file)), $(check_file))
__all:
	$(ECHO) "##########################################################"
	$(ECHO) "Configuration file ".config" not found!                   "
	$(ECHO) "                                                          "
	$(ECHO) "Please run some configurator (e.g. "make oldconfig" or    "
	$(ECHO) "make menuconfig or make xconfig).                         "
	$(ECHO) "##########################################################"
else

ifdef CONFIG_PRELOAD
__all: preload
preload: boot
else
__all: boot
endif

# Support for using generic headers in asm-generic
asm-generic := -f scripts/asm-generic.mk obj

PHONY += asm-generic
asm-generic: FORCE
	$(Q)$(MAKE) $(asm-generic)=arch/$(arch)/include/generated/asm/ \
		generic=include/asm-generic/
	$(Q)$(MAKE) $(asm-generic)=arch/$(arch)/include/generated/lightcore/asm/ \
		generic=include/lightcore/asm-generic/

# Build kernel built-in.a
$(kernel-y): asm-generic dts FORCE
	$(Q)$(MAKE) $(build)=$@
	$(Q)$(RANLIB) $(patsubst %,%built-in.a,$@)

# Link a setof built-in.a to kernel.o
kernel-subfile := $(patsubst %/,%/built-in.a,$(kernel-y))
quiet_cmd_link_kernel = $(ECHO_LD)  $@
      cmd_link_kernel = $(LD) $(ldflags-y) -r -o kernel.o \
                            --whole-archive $(kernel-subfile)
kernel.o: $(kernel-y) scripts_basic
	$(call if_changed,link_kernel)

# Link kernel.o to kernel
ldscripts := arch/$(arch)/kernel.lds
quiet_cmd_make_kernel = $(ECHO_ELF)  $@
      cmd_make_kernel = $(MKELF) $(kernel-flags-y) -T \
                            $(ldscripts) -o $@ $< -lgcc
lightcore: kernel.o
	$(call if_changed,make_kernel)

uboot boot: lightcore FORCE
	$(Q)$(MAKE) $(build)=boot/ $@
preload dts run install: FORCE
	$(Q)$(MAKE) $(build)=boot/ $@

endif

###
# Cleaning is done on three levels.
# make clean     Delete most generated files
#                Leave enough to build external modules
# make mrproper  Delete the current configuration, and all generated files
# make distclean Remove editor backup files, patch leftover files and the like

# clean
clean-subdir-y += $(kernel-y)
clean-y += lightcore kernel.o
clean-y += lightcore.map

bootclean: FORCE
	$(Q)$(MAKE) $(clean)=boot/

usrclean: FORCE
	$(Q)$(MAKE) $(clean)=usr/

clean: bootclean usrclean FORCE
	$(Q)$(MAKE) $(clean)=

# mrproper
MRPROPER_DIRS   += include/config include/generated
MRPROPER_FILES  += .config .config.old tags TAGS cscope* GPATH GTAGS GRTAGS GSYMS

rm-dirs  := $(wildcard $(MRPROPER_DIRS))
rm-files := $(wildcard $(MRPROPER_FILES))
mrproper-dirs := $(addprefix _mrproper_,$(rm-dirs))
mrproper-files := $(addprefix _mrproper_,$(rm-files))

PHONY += $(mrproper-dirs)
$(mrproper-dirs):
	$(Q)$(ECHO) "  $(ECHO_RMDIR) $(patsubst _mrproper_%,%,$@)"
	$(Q)$(RMDIR) $(patsubst _mrproper_%,%,$@)

PHONY += $(mrproper-files)
$(mrproper-files):
	$(Q)$(ECHO) "  $(ECHO_RM) $(patsubst _mrproper_%,%,$@)"
	$(Q)$(RM) $(patsubst _mrproper_%,%,$@)

PHONY += mrproper
mrproper: $(mrproper-files) $(mrproper-dirs) clean
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)

# destclean
distclean: mrproper
	$(Q)$(MAKE) $(clean)=scripts/
	$(Q)$(MAKE) $(clean)=tools/

PHONY += help
help:
	@echo  'Other generic targets:'
	@echo  '  all		  - Build all targets marked with [*]'
	@echo  '* vmlinux	  - Build the bare kernel'
	@echo  '* modules	  - Build all modules'
	@echo  '  modules_install - Install all modules to INSTALL_MOD_PATH (default: /)'
	@echo  '  dir/            - Build all files in dir and below'
	@echo  '  dir/file.[ois]  - Build specified target only'
	@echo  '  dir/file.ll     - Build the LLVM assembly file'
	@echo  '                    (requires compiler support for LLVM assembly generation)'
	@echo  '  dir/file.lst    - Build specified mixed source/assembly target only'
	@echo  '                    (requires a recent binutils and recent build (System.map))'
	@echo  '  dir/file.ko     - Build module including final link'
	@echo  '  modules_prepare - Set up for building external modules'
	@echo  '  tags/TAGS	  - Generate tags file for editors'
	@echo  '  cscope	  - Generate cscope index'
	@echo  '  gtags           - Generate GNU GLOBAL index'
	@echo  '  kernelrelease	  - Output the release version string (use with make -s)'
	@echo  '  kernelversion	  - Output the version stored in Makefile (use with make -s)'
	@echo  '  image_name	  - Output the image name (use with make -s)'
	@echo  '  headers_install - Install sanitised kernel headers to INSTALL_HDR_PATH'; \
	 echo  '                    (default: $(INSTALL_HDR_PATH))'; \
	 echo  ''
	@echo  'Cleaning targets:'
	@echo  '  clean		  - Remove most generated files but keep the config and'
	@echo  '                    enough build support to build external modules'
	@echo  '  mrproper	  - Remove all generated files + config + various backup files'
	@echo  '  distclean	  - mrproper + remove editor backup and patch files'
	@echo  ''
	@echo  'Configuration targets:'
	@$(MAKE) -f $(srctree)/scripts/kconfig/Makefile help
	@echo  ''
	@echo  '  make V=0|1 [targets] 0 => quiet build (default), 1 => verbose build'
	@echo  '  make V=2   [targets] 2 => give reason for rebuild of target'
	@echo  '  make W=n   [targets] Enable extra build checks, n=1,2,3 where'
	@echo  '		1: warnings which may be relevant and do not occur too often'
	@echo  '		2: warnings which occur quite often but may still be relevant'
	@echo  '		3: more obscure warnings, can most likely be ignored'
	@echo  '		Multiple levels can be combined with W=12 or W=123'
	@echo  ''
	@echo  'Execute "make" or "make all" to build all targets marked with [*] '
	@echo  'For further info see the ./README file'

###
# External module support.
# When building external modules the kernel used as basis is considered
# read-only, and no consistency checks are made and the make
# system is not used on the basis kernel. If updates are required
# in the basis kernel ordinary make commands (without M=...) must
# be used.
#
# The following are the only valid targets when building external
# modules.
# make M=dir clean     Delete all automatically generated files
# make M=dir modules   Make all modules in specified dir
# make M=dir	       Same as 'make M=dir modules'
# make M=dir modules_install
#                      Install the modules built in the module directory
#                      Assumes install directory is already created

# read saved command lines for existing targets
existing-targets := $(wildcard $(sort $(targets)))

-include $(foreach f,$(existing-targets),$(dir $(f)).$(notdir $(f)).cmd)

# We are always building only modules.
PHONY += FORCE
FORCE:

# Declare the contents of the PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
