bin-always-y += boot.bin
boot.bin-obj-y += bootloader

dump-always-y += boot.dis
boot.dis-obj-y += bootloader

MKELF := $(CROSS_COMPILE)gcc
elf += bootloader
bootloader-flags-y += -nostdlib -T $(obj)boot.ld
bootloader-flags-y += -Wl,--cref,-Map=$@.map
bootloader-obj-y += built-in.o
bootloader-direct-y += -lgcc

include-y += ./

obj-y += boot/startup.o
obj-y += driver/
# obj-y += dramc/
obj-y += lib/
obj-y += boot.o
include-direct-y += $(MAKE_HOME)/arch/arm/v7a/include/

check-y += boot.dis

ifdef CONFIG_BOOTLOADER_BOOT_MODE_NOR
define hook_build
	$(Q)$(SUBMAKE)=$(MAKE_HOME)/tools/sunxi
	$(Q)$(MAKE_HOME)/tools/sunxi/mksunxiboot $(obj)$(bin-always-y)
	dd if=$(obj)$(bin-always-y) of=$(obj)boot_normode.bin bs=512 seek=0
	$(Q)$(CP) $(obj)boot_normode.bin $(home)/boot.bin
endef
endif

ifdef CONFIG_BOOTLOADER_BOOT_MODE_SD
define hook_build
	$(Q)$(SUBMAKE)=$(MAKE_HOME)/tools/sunxi
	$(Q)$(MAKE_HOME)/tools/sunxi/mksunxiboot $(obj)$(bin-always-y)
	dd if=$(obj)$(bin-always-y) of=$(obj)boot_sdmode.bin bs=512 seek=16
	$(Q)$(CP) $(obj)boot_sdmode.bin $(home)/boot.bin
endef
endif

define hook_clean
	$(Q)$(SUBMAKE)=$(MAKE_HOME)/tools/sunxi $@
endef
