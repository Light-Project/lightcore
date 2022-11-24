# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# System top
# ==========================================================================

include scripts/top.mk

#####################################
# Global Config                     #
#####################################

include $(srctree)/platform.mk

sys-include-y += include/kconfig.h                      \
                 include/compiler/compiler-attributes.h \
                 include/compiler/compiler-type.h       \
                 include/compiler/compiler-gcc.h        \
                 include/compiler/compiler.h            \
                 include/compiler/pointer.h             \
                 include/compiler/sections.h            \
                 include/compiler/stringify.h
sys-include-y += include/ arch/$(arch)/                 \
                 arch/$(arch)/include/                  \
                 arch/$(arch)/include/generated/

optimize-flags-$(CONFIG_CC_OPTIMIZE_FOR_DEBUG)       := -O0
optimize-flags-$(CONFIG_CC_OPTIMIZE_FOR_PERFORMANCE) := -O2
optimize-flags-$(CONFIG_CC_OPTIMIZE_FOR_SIZE)        := -Os

stackp-flags-y                               := -fno-stack-protector
stackp-flags-$(CONFIG_STACKPROTECTOR)        := -fstack-protector
stackp-flags-$(CONFIG_STACKPROTECTOR_STRONG) := -fstack-protector-strong

sys-ccflags-y += $(optimize-flags-y) $(stackp-flags-y)
sys-ccflags-$(CONFIG_STRICT) += -Werror

sys-acflags-y  += -fno-pic -fno-pie
sys-cxflags-y  += -nostdinc -fno-builtin -static
sys-cxflags-y  += -fno-common
sys-cxflags-y  += -ffreestanding
sys-ccflags-y  += -std=gnu17
sys-cxxflags-y += -std=gnu++17 -fpermissive -Wno-pointer-arith

# disable pointer signed / unsigned warnings in gcc 4.0
sys-ccflags-y += -Wno-pointer-sign

sys-acflags-y  += -D__KERNEL__
sys-asflags-y  += -D__ASSEMBLY__
sys-ldsflags-y += -D__ASSEMBLY__

asflags-y  := $(strip $(sys-asflags-y) $(platform-asflags-y))
ccflags-y  := $(strip $(sys-ccflags-y) $(platform-ccflags-y))
cxxflags-y := $(strip $(sys-cxxflags-y) $(platform-cxxflags-y))
acflags-y  := $(strip $(sys-acflags-y) $(platform-acflags-y))
cxflags-y  := $(strip $(sys-cxflags-y) $(platform-cxflags-y))
ldsflags-y := $(strip $(sys-ldsflags-y) $(platform-ldsflags-y))
ldflags-y  := $(strip $(sys-ldflags-y) $(platform-ldflags-y))
include-direct-y := $(strip $(sys-include-y) $(platform-include-y))

lightcore-flags-y += -nostdlib --build-id=sha1
lightcore-flags-y += -e boot_head -T arch/$(arch)/kernel.lds
lightcore-flags-$(CONFIG_KERNEL_MAP) += -Map=$@.map
lightcore-flags-y += $(platform-elfflags-y)

ifdef CONFIG_KERNEL_DEBUG
acflags-y += -g
lightcore-flags-y += -g
endif

ifdef CONFIG_LD_DEAD_CODE_DATA_ELIMINATION
acflags-y += -ffunction-sections -fdata-sections
lightcore-flags-y += --gc-sections
endif

export CROSS_COMPILE AS CC LD
export include-direct-y
export acflags-y cxflags-y cxxflags-y
export ccflags-y asflags-y cppflags-y
export ldsflags-y ldflags-y

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
# Subproject                        #
#####################################

obj-y += arch/
obj-y += doc/logo/
obj-y += drivers/
obj-y += fs/
obj-y += init/
obj-y += ipc/
obj-y += kernel/
obj-y += lib/
obj-y += mm/
obj-y += net/
obj-$(CONFIG_KUSR) += usr/

#####################################
# Generate romdisk                  #
#####################################

$(obj)/boot/romdisk.cpio: $(src)/boot/romdisk/ FORCE
	$(call if_changed,cpio)

ifdef CONFIG_ROMDISK
$(obj)/drivers: $(obj)/boot/romdisk.cpio FORCE
targets += boot/romdisk.cpio
endif

#####################################
# Compiler                          #
#####################################

lightcore-objs += built-in.o
elf-always-y += lightcore

lightcore.dump-obj += lightcore
dump-always-$(CONFIG_KERNEL_DUMP) += lightcore.dump

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
