# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# System define
# ==========================================================================

########################################
# Build toolchain                      #
########################################

CROSS_COMPILE    ?=

#
# Gcc toolchain
CPP         := $(CROSS_COMPILE)cpp
AS          := $(CROSS_COMPILE)gcc
CC          := $(CROSS_COMPILE)gcc
LD          := $(CROSS_COMPILE)ld
CXX         := $(CROSS_COMPILE)g++
AR          := $(CROSS_COMPILE)ar
NM          := $(CROSS_COMPILE)nm
STRIP       := $(CROSS_COMPILE)strip
RANLIB      := $(CROSS_COMPILE)gcc-ranlib
OBJCOPY     := $(CROSS_COMPILE)objcopy
OBJDUMP     := $(CROSS_COMPILE)objdump

AS_FLAGS        ?=
LD_FLAGS        ?=
CC_FLAGS        ?=
CPP_FLAGS       ?=
AR_FLAGS        ?=
NM_FLAGS        ?=
STRIP_FLAGS     ?=
OBJCOPY_FLAGS   ?=
OBJDUMP_FLAGS   ?=

#
# Host toolchain
HOSTCC          ?= gcc
HOSTCXX         ?= g++
HOSTCFLAGS      ?= -Wall
HOSTCXXFLAGS    ?= -O2

MKELF           ?= $(CROSS_COMPILE)gcc

########################################
# CMD tool                             #
########################################

#
# CMD tool
MKDIR           := mkdir -p
MKCPIO          := cpio -o -H newc
CP              := cp -af
DD              := dd
RM              := rm
RMDIR           := rm -r
LN              := ln -fsn
CD              := cd
MV              := mv
FIND            := find
ECHO            := echo -e " "
PERL            := perl
CONFIG_SHELL    := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
                    else if [ -x /bin/bash ]; then echo /bin/bash; \
                    else echo sh; fi ; fi)

INSTALL         = install
BASH            = bash
KGZIP           = gzip
KBZIP2          = bzip2
KLZOP           = lzop
LZMA            = lzma
LZ4             = lz4c
XZ              = xz
ZSTD            = zstd
CHECK           = sparse

define new
$1 ?= $2
endef

########################################
# Echo tips                            #
########################################

########################
# font:
# \e[31m : red
# \e[32m : light green
# \e[33m : orange
# \e[34m : blue
# \e[35m : purple
# \e[36m : dark green
# \e[37m : white
#
# background:
# \e[40m : black
# \e[41m : dark red
# \e[42m : light green
# \e[43m : orange
# \e[44m : blue
# \e[45m : purple
# \e[46m : dark green
# \e[47m : white
########################

ECHO_CPP        := \e[32mCPP\e[0m
ECHO_CC        	:= \e[32mCC\e[0m
ECHO_CXX        := \e[32mCXX\e[0m
ECHO_AS         := \e[32mAS\e[0m
ECHO_AR         := \e[35mAR\e[0m
ECHO_LD         := \e[35mLD\e[0m
ECHO_LDS        := \e[35mLDS\e[0m

ECHO_DTB        := \e[36mDTB\e[0m

ECHO_CUSTCPP    := \e[33mCUSTCPP\e[0m
ECHO_CUSTAS     := \e[33mCUSTAS\e[0m
ECHO_CUSTCC     := \e[33mCUSTCC\e[0m
ECHO_CUSTCXX    := \e[33mCUSTCXX\e[0m
ECHO_CUSTLD     := \e[35mCUSTLD\e[0m

ECHO_HOSTCC     := \e[33mHOSTCC\e[0m
ECHO_HOSTCXX    := \e[33mHOSTCXX\e[0m
ECHO_HOSTLD     := \e[35mHOSTLD\e[0m
ECHO_HOSTLLD    := \e[35mHOSTLLD\e[0m

ECHO_DUMP       := \e[36mDUMP\e[0m
ECHO_NASM       := \e[34mNASM\e[0m
ECHO_ELF        := \e[34mMKELF\e[0m
ECHO_BIN        := \e[34mMKBIN\e[0m

ECHO_RM         := \e[31mRM\e[0m
ECHO_RMDIR      := \e[31mRMDIR\e[0m
ECHO_CLEAN      := \e[31mCLEAN\e[0m
ECHO_CLEAN_DIR  := \e[31mCLEANDIR\e[0m

ECHO_DONE       := \e[5m\e[34mDONE\e[0m
ECHO_OUTPUT     := \e[5m\e[34mOUTPUT\e[0m

ECHO_CHECK      := \e[5m\e[33mCHECK\e[0m

########################################
# Build tool define                    #
########################################

# Shorthand for $(Q)$(MAKE) -f scripts/build.mk obj=
# Usage:
# $(Q)$(MAKE) $(build)=dir
build   := -f scripts/build.mk obj

###
# Shorthand for $(Q)$(MAKE) -f scripts/clean.mk obj=
# Usage:
# $(Q)$(MAKE) $(clean)=dir
clean   := -f scripts/clean.mk obj

########################################
# Build option                         #
########################################

###
# real prerequisites without phony targets
real-prereqs = $(filter-out $(PHONY), $^)

# as-option
# Usage: cflags-y += $(call as-option,-Wa$(comma)-isa=foo,)
as-option = $(call try-run,\
    $(CC) $(BUILD_CFLAGS) $(1) -c -x assembler /dev/null -o "$$TMP",$(1),$(2))

# as-instr
# Usage: cflags-y += $(call as-instr,instr,option1,option2)
as-instr = $(call try-run,\
    printf "%b\n" "$(1)" | $(CC) $(BUILD_AFLAGS) -c -x assembler -o "$$TMP" -,$(2),$(3))

# cc-option
# Usage: cflags-y += $(call cc-option,-march=winchip-c6,-march=i586)
cc-option = $(call try-run,\
    $(CC) $(KBUILD_CPPFLAGS) $(BUILD_CFLAGS) $(1) -c -xc /dev/null -o "$$TMP",$(1),$(2))

# cc-option-yn
# Usage: flag := $(call cc-option-yn,-march=winchip-c6)
cc-option-yn = $(call try-run,\
    $(CC) $(KBUILD_CPPFLAGS) $(BUILD_CFLAGS) $(1) -c -xc /dev/null -o "$$TMP",y,n)

# cc-option-align
# Prefix align with either -falign or -malign
cc-option-align = $(subst -functions=0,,\
    $(call cc-option,-falign-functions=0,-malign-functions=0))

# cc-disable-warning
# Usage: cflags-y += $(call cc-disable-warning,unused-but-set-variable)
cc-disable-warning = $(call try-run,\
    $(CC) $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) -W$(strip $(1)) -c -xc /dev/null -o "$$TMP",-Wno-$(strip $(1)))

# cc-version
# Usage gcc-ver := $(call cc-version)
cc-version = $(shell $(CONFIG_SHELL) $(srctree)/scripts/tools/gcc-version.sh $(CC))

# cc-ifversion
# Usage:  EXTRA_CFLAGS += $(call cc-ifversion, -lt, 0402, -O1)
cc-ifversion = $(shell [ $(call cc-version, $(CC)) $(1) $(2) ] && echo $(3))

# cc-ldoption
# Usage: ldflags += $(call cc-ldoption, -Wl$(comma)--hash-style=both)
cc-ldoption = $(call try-run,\
    $(CC) $(1) -nostdlib -xc /dev/null -o "$$TMP",$(1),$(2))

# hostcc-option
# Usage: cflags-y += $(call hostcc-option,-march=winchip-c6,-march=i586)
hostcc-option = $(call __cc-option, $(HOSTCC),\
    $(HOSTCFLAGS) $(HOST_EXTRACFLAGS),$(1),$(2))

########################################
# conifg definitions                   #
########################################

# Convenient variables
comma           := ,
quote           := "
squote          := '
empty           :=
space           := $(empty) $(empty)
space_escape    := _-_SPACE_-_
pound           := \#

###
# Name of target with a '.' as filename prefix. foo/bar.o => foo/.bar.o
dot-target = $(dir $@).$(notdir $@)

###
# The temporary file to save gcc -MD generated dependencies must not
# contain a comma
depfile = $(subst $(comma),_,$(dot-target).d)

###
# filename of target with directory and extension stripped
basetarget = $(basename $(notdir $@))

###
# filename of first prerequisite with directory and extension stripped
baseprereq = $(basename $(notdir $<))

###
# Escape single quote for use in echo statements
escsq    = $(subst $(squote),'\$(squote)',$1)

# Find all -I options and call addtree
flags    = $(foreach o,$($(1)),$(if $(filter -I%,$(o)),$(call addtree,$(o)),$(o)))

###
# Easy method for doing a status message
       kecho := :
 quiet_kecho := echo
silent_kecho := :
kecho := $($(quiet)kecho)

########################################
# try-run                              #
########################################

# Usage: option = $(call try-run, $(CC)...-o "$$TMP",option-ok,otherwise)
# Exit code chooses option. "$$TMP" is can be used as temporary file and
# is automatically cleaned up.
try-run = $(shell set -e;           \
    TMP="$(TMPOUT).$$$$.tmp";       \
    TMPO="$(TMPOUT).$$$$.o";        \
    if ($(1)) >/dev/null 2>&1;      \
    then echo "$(2)";               \
    else echo "$(3)";               \
    fi;                             \
    rm -f "$$TMP" "$$TMPO")

#######################################
# filechk is used to check if the content of a generated file is updated.
# Sample usage:
# define filechk_sample
#    echo $KERNELRELEASE
# endef
# version.h : Makefile
#    $(call filechk,sample)
# The rule defined shall write to stdout the content of the new file.
# The existing file will be compared with the new one.
# - If no file exist it is created
# - If the content differ the new file is used
# - If they are equal no change, and no timestamp update
# - stdin is piped in from the first prerequisite ($<) so one has
#   to specify a valid file as first prerequisite (often the kbuild file)
define filechk
    $(Q)set -e;                             \
    $(kecho) '  CHK     $@';                \
    mkdir -p $(dir $@);                     \
    $(filechk_$(1)) < $< > $@.tmp;          \
    if [ -r $@ ] && cmp -s $@ $@.tmp; then  \
        rm -f $@.tmp;                       \
    else                                    \
        $(kecho) '  UPD     $@';            \
        mv -f $@.tmp $@;                    \
    fi
endef

########################################
# Auxiliary tool                       #
########################################

# Useful for describing the dependency of composite objects
# Usage:
#   $(call multi_depend, multi_used_targets, suffix_to_remove, suffix_to_add)
define multi_depend
$(foreach m, $(notdir $1), \
    $(eval $(obj)$m: \
    $(addprefix $(obj), $(foreach s, $3, $($(m:%$(strip $2)=%$(s)))))))
endef

# echo command.
# Short version is used, if $(quiet) equals `quiet_', otherwise full one.
echo-cmd = $(if $($(quiet)cmd_$(1)),\
    $(ECHO) '  $(call escsq,$($(quiet)cmd_$(1)))$(echo-why)';)

# printing commands
cmd = $(Q)$(echo-cmd) $(cmd_$(1))

# output directory for tests below
TMPOUT := $(if $(KBUILD_EXTMOD),$(firstword $(KBUILD_EXTMOD))/)

########################################
# Check changed                        #
########################################

# Check if both commands are the same including their order. Result is empty
# string if equal. User may override this check using make KBUILD_NOCMDDEP=1
cmd-check = $(if $(strip $(cmd_$@)),,1)

# Find any prerequisites that are newer than target or that do not exist.
# (This is not true for now; $? should contain any non-existent prerequisites,
# but it does not work as expected when .SECONDARY is present. This seems a bug
# of GNU Make.)
# PHONY targets skipped in both cases.
newer-prereqs = $(filter-out $(PHONY),$?)

# Replace >$< with >$$< to preserve $ when reloading the .cmd file
# (needed for make)
# Replace >#< with >$(pound)< to avoid starting a comment in the .cmd file
# (needed for make)
# Replace >'< with >'\''< to be able to enclose the whole string in '...'
# (needed for the shell)
make-cmd = $(call escsq,$(subst $(pound),$$(pound),\
            $(subst $$,$$$$,$(cmd_$(1)))))

# Execute command if command has changed or prerequisite(s) are updated.
if_changed = $(if $(strip $(newer-prereqs) $(cmd-check)),                   \
    $(Q)set -e;                                                             \
    $(echo-cmd) $(cmd_$(1));                                                \
    printf '%s\n' 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd, @:)

# Execute the command and also postprocess generated .d dependencies file.
if_changed_dep = $(if $(strip $(newer-prereqs) $(cmd-check) ),              \
    $(Q)set -e;                                                             \
    $(echo-cmd) $(cmd_$(1));                                                \
    scripts/basic/fixdep $(depfile) $@ '$(make-cmd)' > $(dot-target).tmp;   \
    rm -f $(depfile);                                                       \
    mv -f $(dot-target).tmp $(dot-target).cmd)

cmd_and_fixdep =                                                            \
    $(cmd);                                                                 \
    scripts/basic/fixdep $(depfile) $@ '$(make-cmd)' > $(dot-target).cmd;   \
    rm -f $(depfile)

# Usage: $(call if_changed_rule,foo)
# Will check if $(cmd_foo) or any of the prerequisites changed,
# and if so will execute $(rule_foo).
if_changed_rule = $(if $(strip $(newer-prereqs) $(cmd-check) ),             \
    $(Q)set -e;                                                             \
    $(rule_$(1)), @:)

#########################################
# why - tell why a a target got build
#       enabled by make V=2
#       Output (listed in the order they are checked):
#          (1) - due to target is PHONY
#          (2) - due to target missing
#          (3) - due to: file1.h file2.h
#          (4) - due to command line change
#          (5) - due to missing .cmd file
#          (6) - due to target not in $(targets)
# (1) PHONY targets are always build
# (2) No target, so we better build it
# (3) Prerequisite is newer than target
# (4) The command line stored in the file named dir/.target.cmd
#     differed from actual command line. This happens when compiler
#     options changes
# (5) No dir/.target.cmd file (used to store command line)
# (6) No dir/.target.cmd file and target not listed in $(targets)
#     This is a good hint that there is a bug in the kbuild file
ifeq ($(KBUILD_VERBOSE),2)
why =                                                                       \
    $(if $(filter $@, $(PHONY)),- due to target is PHONY,                   \
        $(if $(wildcard $@),                                                \
            $(if $(strip $(newer-prereqs)),- due to: $(newer-prereqs),      \
                $(if $(arg-check),                                          \
                    $(if $(cmd_$@),- due to command line change,            \
                        $(if $(filter $@, $(targets)),                      \
                            - due to missing .cmd file,                     \
                            - due to $(notdir $@) not in $$(targets)        \
                         )                                                  \
                     )                                                      \
                 )                                                          \
             ),                                                             \
             - due to target missing                                        \
         )                                                                  \
     )

echo-why = $(call escsq, $(strip $(why)))
endif
