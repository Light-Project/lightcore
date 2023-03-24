# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
#

sys-include-y += include/kconfig.h                      \
                 include/compiler/compiler-attributes.h \
                 include/compiler/compiler-type.h       \
                 include/compiler/compiler-gcc.h        \
                 include/compiler/compiler.h            \
                 include/compiler/incbin.h              \
                 include/compiler/pointer.h             \
                 include/compiler/sections.h            \
                 include/compiler/stringify.h
sys-include-y += include/ arch/$(arch)/                 \
                 arch/$(arch)/include/                  \
                 arch/$(arch)/include/generated/

optimize-flags-$(CONFIG_CC_OPTIMIZE_FOR_DEBUG)       := -O0
optimize-flags-$(CONFIG_CC_OPTIMIZE_FOR_PERFORMANCE) := -O2
optimize-flags-$(CONFIG_CC_OPTIMIZE_FOR_SIZE)        := -Os

stackp-flags-y := -fno-stack-protector
stackp-flags-$(CONFIG_STACKPROTECTOR) := -fstack-protector
stackp-flags-$(CONFIG_STACKPROTECTOR_STRONG) := -fstack-protector-strong

sys-ccflags-y += $(optimize-flags-y) $(stackp-flags-y)
sys-ccflags-$(CONFIG_STRICT) += -Werror

sys-acflags-y += -fno-pic -fno-pie
sys-cxflags-y += -nostdinc -fno-builtin -static
sys-cxflags-y += -fno-common
sys-cxflags-y += -ffreestanding
sys-ccflags-y += -std=gnu17
sys-cxxflags-y += -std=gnu++17 -fpermissive -Wno-pointer-arith

# disable pointer signed / unsigned warnings in gcc 4.0
sys-ccflags-y += -Wno-pointer-sign

sys-acflags-y += -D__KERNEL__
sys-asflags-y += -D__ASSEMBLY__
sys-ldsflags-y += -D__ASSEMBLY__

asflags-y := $(strip $(sys-asflags-y) $(platform-asflags-y))
ccflags-y := $(strip $(sys-ccflags-y) $(platform-ccflags-y))
cxxflags-y := $(strip $(sys-cxxflags-y) $(platform-cxxflags-y))
acflags-y := $(strip $(sys-acflags-y) $(platform-acflags-y))
cxflags-y := $(strip $(sys-cxflags-y) $(platform-cxflags-y))
ldsflags-y := $(strip $(sys-ldsflags-y) $(platform-ldsflags-y))
ldflags-y := $(strip $(sys-ldflags-y) $(platform-ldflags-y))
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
