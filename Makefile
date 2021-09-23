# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# System top
# ==========================================================================

include scripts/top.mk

# ---------------------------------------------------------------------------
# Global Config

include $(srctree)/platform.mk

sys-include-y += include/ include/kconfig.h             \
                 include/compiler/compiler_attributes.h \
                 include/compiler/compiler_type.h       \
                 include/compiler/compiler.h            \
                 include/compiler/sections.h            \
                 include/compiler/stringify.h
sys-include-y += arch/$(arch)/include/                  \
                 arch/$(arch)/include/generated/

sys-acflags-$(CONFIG_CC_OPTIMIZE_FOR_DEBUG)         += -O0
sys-acflags-$(CONFIG_CC_OPTIMIZE_FOR_PERFORMANCE)   += -O2
sys-acflags-$(CONFIG_CC_OPTIMIZE_FOR_SIZE)          += -Os
sys-acflags-$(CONFIG_STRICT) += -Wall -Werror

sys-asflags-y  += -D__ASSEMBLY__
sys-ccflags-y  += -std=gnu11
sys-acflags-y  += -ffreestanding -nostdinc -fno-builtin -static
sys-acflags-y  += -fno-PIE -fno-common
sys-acflags-y  += -fno-stack-protector
sys-ldsflags-y += -D__ASSEMBLY__

asflags-y  := $(strip $(sys-asflags-y) $(platform-asflags-y))
ccflags-y  := $(strip $(sys-ccflags-y) $(platform-ccflags-y))
acflags-y  := $(strip $(sys-acflags-y) $(platform-acflags-y))
ldflags-y  := $(strip $(sys-ldflags-y) $(platform-ldflags-y))
ldsflags-y := $(strip $(sys-ldsflags-y) $(platform-ldsflags-y))
include-direct-y := $(strip $(sys-include-y) $(platform-include-y))

lightcore-flags-$(CONFIG_KERNEL_MAP) += -Wl,--cref,-Map=$@.map
lightcore-flags-y += -e boot_head -T arch/$(arch)/kernel.lds -nostdlib
lightcore-flags-y += $(platform-elfflags-y) -Wl,--build-id=sha1 -lgcc -fno-pic

ifdef CONFIG_KERNEL_DEBUG
acflags-y += -g
lightcore-flags-y += -g
endif

ifdef CONFIG_LD_DEAD_CODE_DATA_ELIMINATION
acflags-y += -ffunction-sections -fdata-sections
lightcore-flags-y += -Wl,--gc-sections
endif

export include-direct-y

# ---------------------------------------------------------------------------
# Generic headers

asm-generic: FORCE
	$(Q)$(MAKE) $(asm-generic)=arch/$(arch)/include/generated/asm \
	generic=include/asm-generic
	$(Q)$(MAKE) $(asm-generic)=arch/$(arch)/include/generated/lightcore/asm \
	generic=include/lightcore/asm-generic

# ---------------------------------------------------------------------------
# Devicetree files

dtstree = boot/dts

%dtb: scripts_dtc
	$(Q)$(MAKE) $(build)=$(dtstree) $(dtstree)/$@
dtbs: scripts_dtc
	$(Q)$(MAKE) $(build)=$(dtstree)

ifdef CONFIG_BUILTIN_DTB
build: dtbs
endif

# ---------------------------------------------------------------------------
# Subproject

obj-y += arch/
obj-y += crypto/
obj-y += doc/
obj-y += drivers/
obj-y += fs/
obj-y += init/
obj-y += ipc/
obj-y += kernel/
obj-y += lib/
obj-y += mm/
obj-y += net/
obj-y += virt/

# ---------------------------------------------------------------------------
# Compiler

lightcore-objs += built-in.o
elf-always-y += lightcore

build: asm-generic scripts_basic FORCE
	$(Q)$(MAKE) $(build)=$(srctree)

kboot: build build/boot/kboot FORCE
preload: build/boot/preload FORCE

disk: preload
disk uboot: kboot FORCE
	$(Q)$(MAKE) $(build)=$(srctree)/boot $@

ifdef CONFIG_PRELOAD
start: disk
else
start: uboot
endif

# ---------------------------------------------------------------------------
# Run & Install

include $(srctree)/boot/run/config.mk
ckfile := $(objtree)/boot/kboot/boot.bin

ifeq ($(wildcard $(ckfile)),)
run install uninsall: FORCE
	$(Q)$(ECHO) "Please run make first"
else
run: FORCE
	$(Q)$(SHELL) $(srctree)/boot/run/$(run_shell)
%install: FORCE
	$(Q)$(SHELL) $(srctree)/boot/install $@
endif

clean-subdir-y += boot/ usr/
