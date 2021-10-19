# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# Platform configure
# ==========================================================================

ifdef CONFIG_ARCH_ARM
CROSS_COMPILE       := arm-none-eabi-
arch                := arm

arch-$(CONFIG_ARCH_ARM_V7)  = -D__ARM_ARCH__=7 $(call cc-option,-march=armv7-a,-march=armv5t -Wa$(comma)-march=armv7-a)
arch-$(CONFIG_ARCH_ARM_V6)  = -D__ARM_ARCH__=6 $(call cc-option,-march=armv6,-march=armv5t -Wa$(comma)-march=armv6)
arch-$(CONFIG_ARCH_ARM_V5)  = -D__ARM_ARCH__=5 $(call cc-option,-march=armv5te,-march=armv4t)
arch-$(CONFIG_ARCH_ARM_V4T) = -D__ARM_ARCH__=4 -march=armv4t
arch-$(CONFIG_ARCH_ARM_V4)  = -D__ARM_ARCH__=4 -march=armv4

platform-acflags-y  += -marm $(arch-y) -msoft-float
platform-elfflags-y += -EL $(arch-y)

endif # CONFIG_ARCH_ARM
CROSS_COMPILE       := aarch64-linux-gnu-
arch                := arm64

ifdef CONFIG_ARCH_ARM64


endif # CONFIG_ARCH_ARM64

ifdef CONFIG_ARCH_CSKY
include arch/csky/config.mk
CROSS_COMPILE       := csky-linux-
arch                := csky
endif # CONFIG_ARCH_CSKY

ifdef CONFIG_ARCH_RISCV
CROSS_COMPILE       := riscv64-linux-gnu-
arch                := riscv64

platform-acflags-y  += -march=rv64imafdc -mabi=lp64d -mcmodel=medany
endif # CONFIG_ARCH_RISCV

ifdef CONFIG_ARCH_X86
CROSS_COMPILE       :=
arch                := x86

ifneq ($(call cc-option, -mpreferred-stack-boundary=2),)
      cc_stack_align4 := -mpreferred-stack-boundary=2
else ifneq ($(call cc-option, -mstack-alignment=4),)
      cc_stack_align4 := -mstack-alignment=4
endif

platform-ccflags-y  += $(cc_stack_align4)
# platform-ccflags-y  += -msoft-float -mregparm=3 -freg-struct-return

platform-acflags-y  += -m32
platform-ccflags-y  += -mno-sse -mno-mmx -mno-sse2 -mno-3dnow
platform-ccflags-y  += $(call cc-option,-mno-avx)
platform-ldflags-y  += -m elf_i386
platform-elfflags-y += -m32 -nostdlib
endif # CONFIG_ARCH_X86

ifdef CONFIG_ARCH_XTENSA
include arch/xtensa/config.mk
arch                := xtensa

platform-ccflags-y  += -mtext-section-literals
platform-acflags-y  += -pipe -mlongcalls
endif # CONFIG_ARCH_XTENSA
