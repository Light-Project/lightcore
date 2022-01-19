# SPDX-License-Identifier: GPL-2.0-or-later
include $(src)/platform.mk
export KASAN_SANITIZE = n

obj-$(CONFIG_ARCH_X86)     += bios/
obj-$(CONFIG_CHIP_ESP32)   += esp32/
obj-$(CONFIG_CHIP_ESP8266) += esp8266/
obj-$(CONFIG_CHIP_SUNIV)   += suniv/
obj-$(CONFIG_CHIP_GX6605S) += gx6605s/

obj-y += lib/
include-y += lib/

preload-asflags-y += -D__ASSEMBLY__
preload-ccflags-y += -std=gnu11 -Wall -Werror
preload-acflags-y += -ffreestanding -nostdinc
preload-acflags-y += -fno-pic -fno-stack-protector
preload-acflags-y += -ffunction-sections -fdata-sections
preload-ldsflags-y += -D__ASSEMBLY__
preload-elfflags-y += -nostdlib -Wl,-Map=$@.map -Wl,--gc-sections

ifdef CONFIG_PRELOAD_DEBUG
preload-acflags-y += -g
preload-elfflags-y += -g
endif

asflags-y   = $(strip $(preload-asflags-y) $(platform-asflags-y))
ccflags-y   = $(strip $(preload-ccflags-y) $(platform-ccflags-y))
acflags-y   = $(strip $(preload-acflags-y) $(platform-acflags-y))
ldflags-y   = $(strip $(preload-ldflags-y) $(platform-ldflags-y))
ldsflags-y  = $(strip $(preload-ldsflags-y) $(platform-ldsflags-y))
preload-flags-y = $(strip $(preload-elfflags-y) $(platform-elfflags-y))

preload-objs += built-in.o
elf-always-y += preload

preload.bin-obj += preload
bin-always-y += preload.bin
