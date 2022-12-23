# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# System top
# ==========================================================================

include scripts/top.mk
include scripts/platform.mk
include scripts/compile.mk

#####################################
# Generic headers                   #
#####################################

asm-generic: FORCE
	$(Q)$(MAKE) $(asm-generic)=arch/$(arch)/include/generated/asm \
	generic=include/asm-generic
	$(Q)$(MAKE) $(asm-generic)=arch/$(arch)/include/generated/lightcore/asm \
	generic=include/lightcore/asm-generic

#####################################
# Devicetree files                  #
#####################################

dtstree = boot/dts

%dtb: scripts_dtc
	$(Q)$(MAKE) $(build)=$(dtstree) $(dtstree)/$@

dtbs: scripts_dtc
	$(Q)$(MAKE) $(build)=$(dtstree)

ifdef CONFIG_BUILTIN_DTB
build: dtbs
endif

#####################################
# Compiler                          #
#####################################

build/boot/kboot: build FORCE
build/boot/preload: asm-generic FORCE

kboot: build/boot/kboot FORCE
preload: build/boot/preload FORCE
tools: build/tools/kernelcrc build/tools/mkincbin FORCE

build: asm-generic scripts_basic tools FORCE
	$(Q)$(MAKE) $(build)=$(srctree)

disk uboot: kboot preload FORCE
	$(Q)$(MAKE) $(build)=$(srctree)/boot $@

ifdef CONFIG_PRELOAD
start: disk
else
start: uboot
endif

#####################################
# Run & Install                     #
#####################################

include $(srctree)/boot/run/config.mk
ckfile := $(objtree)/boot/kboot/boot.bin

ifeq ($(wildcard $(ckfile)),)
run install uninsall: FORCE
	$(Q)$(ECHO) "Please run make first"
else
run: FORCE
	$(Q)$(SHELL) $(srctree)/boot/run/$(run_shell)
install: FORCE
	$(Q)$(SHELL) $(srctree)/boot/install.sh \
		$(KERNELVERSION) $(KBOOT_IMAGE) $(INSTALL_PATH)
endif

clean-subdir-y += boot/
