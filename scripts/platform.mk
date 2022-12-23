# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# Platform configure
# ==========================================================================

ifdef CONFIG_ARCH_X86
KBOOT_IMAGE := $(objtree)/boot/kboot/boot.bin
else
KBOOT_IMAGE := $(objtree)/boot/uImage
endif

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

ifdef CONFIG_ARCH_ARM64
CROSS_COMPILE       := aarch64-linux-gnu-
arch                := arm64

endif # CONFIG_ARCH_ARM64

ifdef CONFIG_ARCH_CSKY
CROSS_COMPILE       := csky-elf-
arch                := csky

ifdef CONFIG_CPU_CK610
CPUTYPE             := ck610
CSKYABI             := abiv1
endif

CSKY_STR            := $(CPUTYPE)$(FPUEXT)$(VDSPEXT)$(TEEEXT)
platform-acflags-y  += -mcpu=$(CPUTYPE) -Wa,-mcpu=$(CSKY_STR)
platform-acflags-y  += -msoft-float -mdiv -fno-tree-vectorize
platform-acflags-y  += -mlittle-endian -mbacktrace
platform-ldflags-y  += -EL
platform-elfflags-y +=
endif # CONFIG_ARCH_CSKY

ifdef CONFIG_ARCH_X86
CROSS_COMPILE       :=
arch                := x86

ifneq ($(call cc-option, -mpreferred-stack-boundary=2),)
cc_stack_align4     := -mpreferred-stack-boundary=2
cc_stack_align8     := -mpreferred-stack-boundary=3
else ifneq ($(call cc-option, -mstack-alignment=4),)
cc_stack_align4     := -mstack-alignment=4
cc_stack_align8     := -mstack-alignment=8
endif

platform-ccflags-y  += -msoft-float
platform-ccflags-y  += -mno-mmx -mno-3dnow
platform-ccflags-y  += -mno-sse -mno-sse2 -mno-avx

ifdef CONFIG_ARCH_X86_32
ifdef CONFIG_STACKPROTECTOR
ifndef CONFIG_SMP
sys-ccflags-y += -mstack-protector-guard=global
else
sys-ccflags-y += -mstack-protector-guard-reg=fs
sys-ccflags-y += -mstack-protector-guard-symbol=__stack_chk_guard
endif # CONFIG_SMP
endif # CONFIG_STACKPROTECTOR

platform-acflags-y  += -m32
platform-acflags-y  += -march=i386
platform-ccflags-y  += $(cc_stack_align4)
platform-ldflags-y  += -m elf_i386
platform-elfflags-y += -m elf_i386
endif # CONFIG_ARCH_X86_32

ifdef CONFIG_ARCH_X86_64
platform-acflags-y  += -m64
platform-acflags-y  += -mno-red-zone
platform-acflags-y  += -mcmodel=kernel
platform-ccflags-y  += $(cc_stack_align8)
platform-ldflags-y  += -m elf_x86_64
platform-elfflags-y += -m elf_x86_64
endif # CONFIG_ARCH_X86_64
endif # CONFIG_ARCH_X86

ifdef CONFIG_ARCH_XTENSA
include arch/xtensa/config.mk
arch                := xtensa

platform-ccflags-y  += -mtext-section-literals
platform-acflags-y  += -pipe -mlongcalls
endif # CONFIG_ARCH_XTENSA
