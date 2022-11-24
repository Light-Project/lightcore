# SPDX-License-Identifier: GPL-2.0
# ==========================================================================
# System define
# ==========================================================================

########################################
# Build toolchain                      #
########################################

CROSS_COMPILE ?=
LLVM ?=

ifneq ($(LLVM),)
ifneq ($(filter %/,$(LLVM)),)
LLVM_PREFIX := $(LLVM)
else ifneq ($(filter -%,$(LLVM)),)
LLVM_SUFFIX := $(LLVM)
endif
endif

# Gcc toolchain
ifneq ($(LLVM),)
CPP             := $(LLVM_PREFIX)clang$(LLVM_SUFFIX) -E
AS              := $(LLVM_PREFIX)clang$(LLVM_SUFFIX) -fintegrated-as
CC              := $(LLVM_PREFIX)clang$(LLVM_SUFFIX)
CXX             := $(LLVM_PREFIX)clang++$(LLVM_SUFFIX)
LD              := $(LLVM_PREFIX)ld.lld$(LLVM_SUFFIX)
AR              := $(LLVM_PREFIX)llvm-ar$(LLVM_SUFFIX)
NM              := $(LLVM_PREFIX)llvm-nm$(LLVM_SUFFIX)
OBJCOPY         := $(LLVM_PREFIX)llvm-objcopy$(LLVM_SUFFIX)
OBJDUMP         := $(LLVM_PREFIX)llvm-objdump$(LLVM_SUFFIX)
STRIP           := $(LLVM_PREFIX)llvm-strip$(LLVM_SUFFIX)
RANLIB          := $(LLVM_PREFIX)llvm-ranlib$(LLVM_SUFFIX)
MKELF           := $(CROSS_COMPILE)ld.lld
else
CPP             := $(CROSS_COMPILE)cpp
AS              := $(CROSS_COMPILE)gcc
CC              := $(CROSS_COMPILE)gcc
CXX             := $(CROSS_COMPILE)g++
LD              := $(CROSS_COMPILE)ld
AR              := $(CROSS_COMPILE)ar
NM              := $(CROSS_COMPILE)nm
STRIP           := $(CROSS_COMPILE)strip
OBJCOPY         := $(CROSS_COMPILE)objcopy
OBJDUMP         := $(CROSS_COMPILE)objdump
RANLIB          := $(CROSS_COMPILE)gcc-ranlib
MKELF           := $(CROSS_COMPILE)ld
endif

# Host toolchain
ifneq ($(LLVM),)
HOSTCC          := $(LLVM_PREFIX)clang$(LLVM_SUFFIX)
HOSTCXX         := $(LLVM_PREFIX)clang++$(LLVM_SUFFIX)
else
HOSTCC          := gcc
HOSTCXX         := g++
endif

AS_FLAGS        ?=
LD_FLAGS        ?=
CC_FLAGS        ?=
CPP_FLAGS       ?=
AR_FLAGS        ?=
NM_FLAGS        ?=
STRIP_FLAGS     ?=
OBJCOPY_FLAGS   ?=
OBJDUMP_FLAGS   ?=

HOSTCFLAGS      ?= -Wall
HOSTCXXFLAGS    ?= -O2

########################################
# CMD tool                             #
########################################

MKDIR           := mkdir -p
CP              := cp -af
DD              := dd
RM              := rm -f
RMDIR           := rm -r
LN              := ln -fsn
CD              := cd
MV              := mv
FIND            := find
ECHO            := echo -e "    "
AWK             := awk -f
SED             := sed
PERL            := perl
PYTHON          := python3
CONFIG_SHELL    := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
                    else if [ -x /bin/bash ]; then echo /bin/bash; \
                    else echo sh; fi ; fi)

INSTALL         = install
BASH            = bash
CPIO            = cpio
KGZIP           = gzip
KBZIP2          = bzip2
KLZOP           = lzop
LZMA            = lzma
LZ4             = lz4c
XZ              = xz
ZSTD            = zstd
YACC            = bison
LEX             = lex
CHECK           = sparse
DTC             = $(build_home)/dtc/dtc

define new
$1 ?= $2
endef

########################################
# Echo tips                            #
########################################

# font:
# \e[31m : red
# \e[32m : light green
# \e[33m : orange
# \e[34m : blue
# \e[35m : purple
# \e[36m : dark green
# \e[37m : white

# background:
# \e[40m : black
# \e[41m : dark red
# \e[42m : light green
# \e[43m : orange
# \e[44m : blue
# \e[45m : purple
# \e[46m : dark green
# \e[47m : white

ECHO_CPP        := \e[32mCPP\e[0m
ECHO_CC         := \e[32mCC\e[0m
ECHO_CXX        := \e[32mCXX\e[0m
ECHO_AS         := \e[32mAS\e[0m
ECHO_AR         := \e[32mAR\e[0m
ECHO_LDS        := \e[32mLDS\e[0m
ECHO_SYM        := \e[32mSYM\e[0m

ECHO_DTB        := \e[36mDTB\e[0m
ECHO_LEX        := \e[36mLEX\e[0m
ECHO_YACC       := \e[36mYACC\e[0m

ECHO_HOSTCC     := \e[32mHOSTCC\e[0m
ECHO_HOSTCXX    := \e[32mHOSTCXX\e[0m
ECHO_HOSTLD     := \e[35mHOSTLD\e[0m
ECHO_HOSTLLD    := \e[35mHOSTLLD\e[0m

ECHO_LD         := \e[35mLD\e[0m
ECHO_ELF        := \e[35mMKELF\e[0m

ECHO_DUMP       := \e[34mDUMP\e[0m
ECHO_BIN        := \e[34mMKBIN\e[0m

ECHO_INCBIN     := \e[33mMKINCBIN\e[0m
ECHO_CPIO       := \e[33mCPIO\e[0m
ECHO_GZIP       := \e[33mGZIP\e[0m
ECHO_BZIP2      := \e[33mBZIP2\e[0m
ECHO_LZMA       := \e[33mLZMA\e[0m
ECHO_LZO        := \e[33mLZO\e[0m
ECHO_LZ4        := \e[33mLZ4\e[0m
ECHO_XZKERN     := \e[33mXZKERN\e[0m
ECHO_XZMISC     := \e[33mXZMISC\e[0m
ECHO_ZSTD       := \e[33mZSTD\e[0m
ECHO_ZSTD22     := \e[33mZSTD22\e[0m

ECHO_RM         := \e[31mRM\e[0m
ECHO_RMDIR      := \e[31mRMDIR\e[0m
ECHO_CLEAN      := \e[31mCLEAN\e[0m
ECHO_CLEAN_DIR  := \e[31mCLEANDIR\e[0m

ECHO_WRAP       := \e[33mWRAP\e[0m

########################################
# Build tool define                    #
########################################

# Shorthand for $(Q)$(MAKE) -f scripts/asm-generic.mk obj=
# Usage:
# $(Q)$(MAKE) $(asm-generic)=dir
asm-generic := -f $(build_home)/asm-generic.mk obj

# Shorthand for $(Q)$(MAKE) -f scripts/build.mk obj=
# Usage:
# $(Q)$(MAKE) $(build)=dir
build       := -f $(build_home)/build.mk obj

# Shorthand for $(Q)$(MAKE) -f scripts/clean.mk obj=
# Usage:
# $(Q)$(MAKE) $(clean)=dir
clean       := -f $(build_home)/clean.mk obj

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

# remove target filename to get target directory
fulltarget = $(shell pwd)/$@

# remove prerequisite filename to get prerequisite directory
fullprereq = $(shell pwd)/$<

# remove target filename to get target directory
dirtarget = $(dir $@)

# remove prerequisite filename to get prerequisite directory
dirprereq = $(dir $<)

# remove target filename to get target directory
filetarget = $(notdir $@)

# remove prerequisite filename to get prerequisite directory
fileprereq = $(notdir $<)

# filename of target with directory and extension stripped
basetarget = $(basename $(filetarget))

# filename of first prerequisite with directory and extension stripped
baseprereq = $(basename $(fileprereq))

# Name of target with a '.' as filename prefix. foo/bar.o => foo/.bar.o
dot-target = $(dir $@).$(notdir $@)

# The temporary file to save gcc -MD generated dependencies must not
# contain a comma
depfile = $(subst $(comma),_,$(dot-target).d)

# real prerequisites without phony targets
real-prereqs = $(filter-out $(PHONY), $^)

# Escape single quote for use in echo statements
escsq    = $(subst $(squote),'\$(squote)',$1)

# Find all -I options and call addtree
flags    = $(foreach o,$($(1)),$(if $(filter -I%,$(o)),$(call addtree,$(o)),$(o)))

# Easy method for doing a status message
       kecho := :
 quiet_kecho := echo
silent_kecho := :
kecho := $($(quiet)kecho)

########################################
# try-run                              #
########################################

# output directory for tests below
TMPOUT := $(if $(KBUILD_EXTMOD),$(firstword $(KBUILD_EXTMOD))/)

# Usage: option = $(call try-run, $(CC)...-o "$$TMP",option-ok,otherwise)
# Exit code chooses option. "$$TMP" is can be used as temporary file and
# is automatically cleaned up.
try-run = $(shell set -e;                   \
            TMP="$(TMPOUT).$$$$.tmp";       \
            TMPO="$(TMPOUT).$$$$.o";        \
            TMPSU="$(TMPOUT).$$$$.su";      \
            if ($(1)) >/dev/null 2>&1;      \
            then echo "$(2)";               \
            else echo "$(3)";               \
            fi;                             \
            rm -f "$$TMP" "$$TMPO" "$$TMPSU")

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
# Build option                         #
########################################

# as-option
# Usage: cflags-y += $(call as-option,-Wa$(comma)-isa=foo,)
as-option = $(call try-run,\
    $(CC) $(KBUILD_CFLAGS) $(1) -c -x assembler /dev/null -o "$$TMP",$(1),$(2))

# as-instr
# Usage: cflags-y += $(call as-instr,instr,option1,option2)
as-instr = $(call try-run,\
    printf "%b\n" "$(1)" | $(CC) $(KBUILD_AFLAGS) -c -x assembler -o "$$TMP" -,$(2),$(3))

# __cc-option
# Usage: MY_CFLAGS += $(call __cc-option,$(CC),$(MY_CFLAGS),-march=winchip-c6,-march=i586)
__cc-option = $(call try-run,\
    $(1) -Werror $(2) $(3) -c -x c /dev/null -o "$$TMP",$(3),$(4))

# cc-option
# Usage: cflags-y += $(call cc-option,-march=winchip-c6,-march=i586)
cc-option = $(call __cc-option, $(CC),\
    $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS),$(1),$(2))

# cc-option-yn
# Usage: flag := $(call cc-option-yn,-march=winchip-c6)
cc-option-yn = $(call try-run,\
    $(CC) -Werror $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) $(1) -c -x c /dev/null -o "$$TMP",y,n)

# cc-disable-warning
# Usage: cflags-y += $(call cc-disable-warning,unused-but-set-variable)
cc-disable-warning = $(call try-run,\
    $(CC) -Werror $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) -W$(strip $(1)) -c -x c /dev/null -o "$$TMP",-Wno-$(strip $(1)))

# cc-ifversion
# Usage:  EXTRA_CFLAGS += $(call cc-ifversion, -lt, 0402, -O1)
cc-ifversion = $(shell [ $(CONFIG_GCC_VERSION)0 $(1) $(2)000 ] && echo $(3) || echo $(4))

# cc-ifversion
# Usage:  EXTRA_CFLAGS += $(call cc-ifversion, -lt, 0402, -O1)
cc-ifversion = $(shell [ $(call cc-version, $(CC)) $(1) $(2) ] && echo $(3))

# ld-option
# Usage: KBUILD_LDFLAGS += $(call ld-option, -X, -Y)
ld-option = $(call try-run, $(LD) $(KBUILD_LDFLAGS) $(1) -v,$(1),$(2),$(3))

# ld-ifversion
# Usage:  $(call ld-ifversion, -ge, 22252, y)
ld-ifversion = $(shell [ $(CONFIG_LD_VERSION)0 $(1) $(2)0 ] && echo $(3) || echo $(4))

# hostcc-option
# Usage: cflags-y += $(call hostcc-option,-march=winchip-c6,-march=i586)
hostcc-option = $(call __cc-option, $(HOSTCC), $(HOSTCFLAGS) $(HOST_EXTRACFLAGS),$(1),$(2))

########################################
# Auxiliary tool                       #
########################################

# Useful for describing the dependency of composite objects
# Usage:
#   $(call multi_depend, multi_used_targets, suffix_to_remove, suffix_to_add)
define multi_depend
$(foreach m, $(notdir $1), \
    $(eval $(obj)/$m: \
    $(addprefix $(obj)/, $(foreach s, $3, $($(m:%$(strip $2)=%$(s)))))))
endef

target-stem = $(basename $(patsubst $(obj)/%,%,$@))

# echo command.
# Short version is used, if $(quiet) equals `quiet_', otherwise full one.
echo-cmd = $(if $($(quiet)cmd_$(1)),$(ECHO) '$(call escsq,$($(quiet)cmd_$(1)))$(echo-why)';)

# printing commands
cmd = @set -e; $(echo-cmd) $($(quiet)redirect) $(cmd_$(1))

# Add intermediate targets:
# When building objects with specific suffix patterns, add intermediate
# targets that the final targets are derived from.
intermediate_targets = $(foreach s, $(3), $(patsubst %$(strip $(2)),%$(s), $(filter %$(strip $(2)), $(1))))

########################################
# Check changed                        #
########################################

# Check if both commands are the same including their order. Result is
# emptystring if equal.
cmd-check = $(filter-out $(subst $(space),$(space_escape),$(strip $(cmd_$@))), \
                         $(subst $(space),$(space_escape),$(strip $(cmd_$1))))

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
make-cmd = $(call escsq,$(subst $(pound),$$(pound), $(subst $$,$$$$,$(cmd_$(1)))))

# Execute command if command has changed or prerequisite(s) are updated.
if_changed = $(if $(newer-prereqs)$(cmd-check),                             \
    $(cmd);                                                                 \
    printf '%s\n' 'cmd_$@ := $(make-cmd)' > $(dot-target).cmd, @:)

# Execute the command and also postprocess generated .d dependencies file.
if_changed_dep = $(if $(newer-prereqs)$(cmd-check),$(cmd_and_fixdep),@:)

cmd_and_fixdep =                                                            \
    $(cmd);                                                                 \
    scripts/basic/fixdep $(depfile) $@ '$(make-cmd)' > $(dot-target).cmd;   \
    rm -f $(depfile)

# Usage: $(call if_changed_rule,foo)
# Will check if $(cmd_foo) or any of the prerequisites changed,
# and if so will execute $(rule_foo).
if_changed_rule = $(if $(newer-prereqs)$(cmd-check),$(rule_$(1)),@:)

########################################
# why changed                          #
########################################

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
ifeq ($(BUILD_VERBOSE),2)
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
