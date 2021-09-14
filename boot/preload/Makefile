# SPDX-License-Identifier: GPL-2.0-or-later
tar-$(CONFIG_ARCH_X86)     += bios/
tar-$(CONFIG_CHIP_ESP32)   += esp32/
tar-$(CONFIG_CHIP_ESP8266) += esp8266/
tar-$(CONFIG_CHIP_SUNIV)   += suniv/
obj-y += $(tar-y) lib/

include-y += $(obj)lib/

include $(obj)platform.mk

preload-asflags-y = -D__ASSEMBLY__
preload-ccflags-y = -std=gnu11
preload-acflags-y = -ffreestanding -nostdinc -Wall -Werror \
                    -fno-PIE  -fno-stack-protector \
                    -ffunction-sections -fdata-sections
preload-ldsflags-y = -D__ASSEMBLY__
preload-elfflags-y = -Wl,--gc-sections -Wl,--cref,-Map=$@.map

asflags-y   = $(strip $(preload-asflags-y) $(platform-asflags-y))
ccflags-y   = $(strip $(preload-ccflags-y) $(platform-ccflags-y))
acflags-y   = $(strip $(preload-acflags-y) $(platform-acflags-y))
ldflags-y   = $(strip $(preload-ldflags-y) $(platform-ldflags-y))
ldsflags-y  = $(strip $(preload-ldsflags-y) $(platform-ldsflags-y))
elfflags-y  = $(strip $(preload-elfflags-y) $(platform-elfflags-y))

quiet_cmd_link_preload = $(ECHO_ELF)  $@
      cmd_link_preload = $(RANLIB) $(obj)built-in.a; \
                         $(LD) $(ldflags-y) -r -o $(obj)built-in.o --whole-archive $(obj)built-in.a; \
                         $(MKELF) $(elfflags-y) -o $@ $(obj)built-in.o $(preload-lib)
$(obj)preload: $(obj)built-in.a FORCE
	$(call if_changed,link_preload)
preload: $(obj)preload

ifneq ($(tar-y),)
always-y += preload
endif
clean-y += preload preload.map built-in.o
clean-subdir-y += $(tar-)
