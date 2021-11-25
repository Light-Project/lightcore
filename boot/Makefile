# SPDX-License-Identifier: GPL-2.0

# ---------------------------------------------------------------------------
# Uboot

MKIMAGE := scripts/tools/mkuboot.sh

ifdef CONFIG_CHIP_X86
UIMAGE_LOADADDR     := 0x10000
UIMAGE_ENTRYADDR    := 0x10200
endif

UIMAGE_ARCH         ?= sandbox
UIMAGE_COMPRESSION  ?= none
UIMAGE_OPTS-y       ?=
UIMAGE_TYPE         ?= kernel
UIMAGE_LOADADDR     ?= $(CONFIG_LOAD_ADDR)
UIMAGE_ENTRYADDR    ?= $(UIMAGE_LOADADDR)
UIMAGE_NAME         ?= 'Lightcore-$(KERNELVERSION)'

quiet_cmd_uimage = UIMAGE $@
      cmd_uimage = $(BASH) $(MKIMAGE) -A $(UIMAGE_ARCH) -O invalid      \
                        -C $(UIMAGE_COMPRESSION) $(UIMAGE_OPTS-y)       \
                        -T $(UIMAGE_TYPE)                               \
                        -a $(UIMAGE_LOADADDR) -e $(UIMAGE_ENTRYADDR)    \
                        -n $(UIMAGE_NAME) -d $< $@
$(obj)/uImage: $(obj)/kboot/boot.bin FORCE
	$(call if_changed,uimage)
uboot: $(obj)/uImage

# ---------------------------------------------------------------------------
# Disk

quiet_cmd_disk = MKDISK $@

ifneq ($(CONFIG_CHIP_X86),)
cmd_disk = $(DD) if=$(obj)/preload/preload.bin of=$(obj)/disk.img bs=512 conv=notrunc; \
           $(DD) if=$(obj)/uImage of=$(obj)/disk.img bs=512 seek=63 conv=notrunc
endif

ifneq ($(CONFIG_CHIP_GX6605S),)
cmd_disk = $(DD) if=$(obj)/preload/preload.bin of=$(obj)/disk.img bs=512 conv=notrunc; \
           $(DD) if=$(obj)/uImage of=$(obj)/disk.img bs=512 seek=16 conv=notrunc
endif

ifneq ($(CONFIG_CHIP_SUNIV),)
cmd_disk = $(MAKE) $(build)=tools/sunxi/; \
           tools/sunxi/mksunxiboot $(obj)/preload.bin; \
           $(DD) if=$(obj)/preload.bin of=$(obj)/disk.img bs=512 conv=notrunc; \
           $(DD) if=$(obj)/uImage of=$(obj)/disk.img bs=512 seek=64 conv=notrunc
endif

ifneq ($(CONFIG_CHIP_ESP8266),)
cmd_disk = tools/esp/esptool.py -c esp8266 elf2image -o $(obj)/preload- $(obj)/preload/preload; \
           tools/esp/esptool.py -c esp8266 image_info $(obj)/preload-0x00000.bin; \
           $(DD) if=$(obj)/preload-0x00000.bin of=$(obj)/disk.img bs=512 conv=notrunc; \
           $(DD) if=$(obj)/kernel.bin of=$(obj)/disk.img bs=512 seek=64 conv=notrunc;
endif

ifneq ($(CONFIG_CHIP_ESP32),)
cmd_disk = tools/esp/esptool.py -c esp32 elf2image -fm dio -o $(obj)/preload.bin $(obj)/preload/preload; \
           tools/esp/esptool.py -c esp32 image_info $(obj)/preload.bin; \
           $(DD) if=/dev/zero of=$(obj)/disk.img bs=512 count=8 conv=notrunc; \
           $(DD) if=$(obj)/preload.bin of=$(obj)/disk.img bs=512 seek=8 conv=notrunc; \
           $(DD) if=$(obj)/kernel.bin of=$(obj)/disk.img bs=512 seek=64 conv=notrunc;
endif

$(obj)/disk.img: $(obj)/preload/preload.bin $(obj)/uImage FORCE
	$(call if_changed,disk)
disk: $(obj)/disk.img

targets += kboot/boot.bin preload/preload.bin uImage disk.img
clean-y += uImage disk.img
clean-subdir-y += kboot/ preload/ dts/
