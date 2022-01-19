# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += core.o
obj-$(CONFIG_KASAN_GENERIC) += generic.o

CCFLAGS_KASAN   += $(call cc-option, -fno-conserve-stack)
CCFLAGS_KASAN   += -fno-stack-protector
CCFLAGS_KASAN   += -DDISABLE_BRANCH_PROFILING
core-flags-y    := $(CCFLAGS_KASAN)
generic-flags-y := $(CCFLAGS_KASAN)
