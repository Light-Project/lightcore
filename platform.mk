#############################################################
# Platform Config
#############################################################

ifdef CONFIG_ARCH_ARM
CROSS_COMPILE       := arm-none-eabi-
arch                := arm

platform-acflags-y  += -D__ARCH_BIT32__
platform-eflags-y   += -nostdlib

arch-$(CONFIG_ARCH_ARM_V6)  = $(call cc-option,-march=armv7-a,-march=armv5t -Wa$(comma)-march=armv7-a)
arch-$(CONFIG_ARCH_ARM_V6)  = $(call cc-option,-march=armv6,-march=armv5t -Wa$(comma)-march=armv6)
arch-$(CONFIG_ARCH_ARM_V5)  = $(call cc-option,-march=armv5te,-march=armv4t)
arch-$(CONFIG_ARCH_ARM_V4T) = -march=armv4t
arch-$(CONFIG_ARCH_ARM_V4)  = -march=armv4

platform-acflags-y += $(arch-y)

endif # CONFIG_ARCH_ARM

ifdef CONFIG_ARCH_ARME
CROSS_COMPILE       := arm-none-eabi-
arch                := arme

platform-acflags-y  += -march=armv7-m -mthumb
platform-eflags-y   += -nostdlib -mthumb 
endif

ifdef CONFIG_ARCH_CSKY
include arch/csky/config.mk
CROSS_COMPILE       := /disk/d/project/buildroot/output/host/bin/csky-linux-
arch                := csky

platform-eflags-y   += -nostdlib
endif

ifdef CONFIG_ARCH_RISCV
CROSS_COMPILE       := riscv64-linux-gnu-
arch                := riscv64

platform-acflags-y  := -march=rv64imafdc -mabi=lp64d -mcmodel=medany
platform-eflags-y   += -nostdlib
endif

ifdef CONFIG_ARCH_X86
CROSS_COMPILE       := 
arch                := x86

ifneq ($(call cc-option, -mpreferred-stack-boundary=2),)
      cc_stack_align4 := -mpreferred-stack-boundary=2
else ifneq ($(call cc-option, -mstack-alignment=4),)
      cc_stack_align4 := -mstack-alignment=4
endif

biarch := $(call cc-option,-m32)

platform-acflags-y  += -m32

platform-ccflags-y  += $(cc_stack_align4)

# Try to use registers to pass parameters
# platform-ccflags-y  += -mregparm=3

# Prevent GCC from generating any FP code by mistake.
platform-ccflags-y  += -mno-sse -mno-mmx -mno-sse2 -mno-3dnow 
platform-ccflags-y  += $(call cc-option,-mno-avx)

platform-ldflags-y  += -m elf_i386
platform-eflags-y   += -m32 -nostdlib
endif

ifdef CONFIG_ARCH_XTENSA
include arch/xtensa/config.mk
arch                := xtensa

platform-ccflags-y  += -mtext-section-literals
platform-acflags-y  += -pipe -mlongcalls 
platform-eflags-y   += -nostdlib
endif

export platform-asflags-y platform-ccflags-y platform-acflags-y 
export platform-ldflags-y platform-ldsflags-y platform-eflags-y
