# SPDX-License-Identifier: GPL-2.0-or-later

include $(src)/dts/config.mk
include $(src)/run/config.mk

# DTS
DTC ?= scripts/dtc/dtc
dtb := $(addprefix $(obj)/, $(dtb-y))

DTC: FORCE
	$(Q)$(MAKE) $(build)=scripts/dtc

dtc_cpp_flags += -I include -undef -D__DTS__ -nostdinc

quiet_cmd_dts_o_dtb = $(ECHO_DTB) $@
      cmd_dts_o_dtb = $(HOSTCC) -E $(dtc_cpp_flags) -x assembler-with-cpp -o $(dtc-tmp) $< ;    \
                      $(DTC) -o $@ -b 0                                                         \
                      $(addprefix -i,$(dir $<) $(DTC_INCLUDE)) $(DTC_FLAGS)                     \
                        -d $(depfile).dtc.tmp $(dtc-tmp) ;                                      \
                        cat $(depfile).dtc.tmp > $(depfile)
                        
dtc-tmp = $(subst $(comma),_,$(dot-target).dts.tmp)

$(obj)/%.dtb: $(src)/dts/%.dts DTC FORCE
	$(call cmd_and_fixdep,dts_o_dtb)

dts: $(dtb) FORCE

targets += $(dtb)
clean-y += *.dtb .*.dtc.tmp .*.dts.tmp

# KBOOT
quiet_cmd_copy_kernel = OBJCOPY $@
      cmd_copy_kernel = $(OBJCOPY) -O binary $< $@
$(obj)/kernel.bin.tmp: lightcore FORCE
	$(call if_changed,copy_kernel)

fixtool: FORCE
	$(Q)$(MAKE) $(build)=tools/kernelcrc

quiet_cmd_fix_kernel = FIXCRC $@
      cmd_fix_kernel = tools/kernelcrc/kernelcrc $<
$(obj)/kernel.bin: $(obj)/kernel.bin.tmp
	$(call if_changed,fix_kernel)
	$(Q)$(MV) $< $@
$(obj)/kernel.bin: fixtool

$(obj)/kernel.bin.gz: $(obj)/kernel.bin FORCE
	$(call if_changed,gzip)
$(obj)/kernel.bin.bz2: $(obj)/kernel.bin FORCE
	$(call if_changed,bzip2)
$(obj)/kernel.bin.lzma: $(obj)/kernel.bin FORCE
	$(call if_changed,lzma)
$(obj)/kernel.bin.xz: $(obj)/kernel.bin FORCE
	$(call if_changed,xzkern)
$(obj)/kernel.bin.lzo: $(obj)/kernel.bin FORCE
	$(call if_changed,lzo)
$(obj)/kernel.bin.lz4: $(obj)/kernel.bin FORCE
	$(call if_changed,lz4)
$(obj)/kernel.bin.zst: $(obj)/kernel.bin FORCE
	$(call if_changed,zstd22)

suffix-$(CONFIG_PACK_GZIP)    := .gz
suffix-$(CONFIG_PACK_BZIP2)   := .bz2
suffix-$(CONFIG_PACK_LZMA)    := .lzma
suffix-$(CONFIG_PACK_XZ)      := .xz
suffix-$(CONFIG_PACK_LZO)     := .lzo
suffix-$(CONFIG_PACK_LZ4)     := .lz4
suffix-$(CONFIG_PACK_ZSTD)    := .zst

quiet_cmd_copy_boot = OBJCOPY $@
      cmd_copy_boot = $(OBJCOPY) -O binary $(obj)/kboot/boot $(obj)/boot.bin
$(obj)/boot.bin: $(obj)/kernel.bin$(suffix-y) 
ifndef CONFIG_XIP
	$(Q)$(CP) $(obj)/kernel.bin$(suffix-y) $(obj)/piggy_data
	$(Q)$(MAKE) $(build)=$(src)/kboot boot
	$(call if_changed,copy_boot)
endif
boot: $(obj)/boot.bin

targets += $(obj)/kernel.bin $(obj)/kernel.bin$(suffix-y)

clean-suffix += .gz .bz2 .lzma .xz .lzo .lz4 .zst
clean-y += $(addprefix kernel.bin,$(clean-suffix))
clean-y += kernel.bin piggy_data boot boot.bin

# UBOOT
MKIMAGE := $(srctree)/scripts/mkuboot.sh

UIMAGE_ARCH         = sandbox
UIMAGE_COMPRESSION  = none
UIMAGE_OPTS-y       =
UIMAGE_TYPE         = kernel
UIMAGE_LOADADDR     = $(shell $(NM) $(obj)/kboot/boot | awk '$$NF == "_start" {print $$1}')
UIMAGE_ENTRYADDR    = $(UIMAGE_LOADADDR)
UIMAGE_NAME         = 'Lightcore-$(KERNELVERSION)'

quiet_cmd_uimage = UIMAGE  $@
      cmd_uimage = $(BASH) $(MKIMAGE) -A $(UIMAGE_ARCH) -O invalid      \
                        -C $(UIMAGE_COMPRESSION) $(UIMAGE_OPTS-y)       \
                        -T $(UIMAGE_TYPE)                               \
                        -a $(UIMAGE_LOADADDR) -e $(UIMAGE_ENTRYADDR)    \
                        -n $(UIMAGE_NAME) -d $< $@

$(obj)/uImage: $(obj)/boot.bin
	$(call if_changed,uimage)
	@echo 'uImage: $@ is ready'

uboot: $(obj)/uImage

targets += $(obj)/uImage
clean-y += uImage

# PRELOAD
quiet_cmd_copy_preload = OBJCOPY $@
      cmd_copy_preload = $(OBJCOPY) -O binary $(obj)/preload/preload $(obj)/preload.bin
      
$(obj)/preload.bin: FORCE
	$(Q)$(MAKE) $(build)=$(src)/preload
	$(call if_changed,copy_preload)

preload: $(obj)/preload.bin

ifneq ($(CONFIG_CHIP_X86),)
preload:
	$(Q)$(DD) if=$(obj)/preload.bin of=$(obj)/disk.img bs=512 conv=notrunc
	$(Q)$(DD) if=$(obj)/boot.bin of=$(obj)/disk.img bs=512 seek=63 conv=notrunc
endif
ifneq ($(CONFIG_CHIP_SUNXIF1)$(CONFIG_CHIP_SUNXIH3),)
preload:
	$(Q)$(MAKE) $(build)=tools/sunxi
	$(Q) tools/sunxi/mksunxiboot $(obj)/preload.bin
endif
ifneq ($(CONFIG_CHIP_ESP8266),)
preload:
	$(Q)tools/esp/esptool.py -c esp8266 elf2image -o $(obj)/preload- $(obj)/preload/preload
	$(Q)tools/esp/esptool.py -c esp8266 image_info $(obj)/preload-0x00000.bin
	$(Q)$(DD) if=$(obj)/preload-0x00000.bin of=$(obj)/disk.img bs=512 conv=notrunc
	$(Q)$(DD) if=$(obj)/kernel.bin of=$(obj)/disk.img bs=512 seek=64 conv=notrunc
endif
ifneq ($(CONFIG_CHIP_ESP32),)
preload:
	$(Q)tools/esp/esptool.py -c esp32 elf2image -fm dio -o $(obj)/preload.bin $(obj)/preload/preload
	$(Q)tools/esp/esptool.py -c esp32 image_info $(obj)/preload.bin 
	$(Q)$(DD) if=/dev/zero of=$(obj)/disk.img bs=512 count=8 conv=notrunc
	$(Q)$(DD) if=$(obj)/preload.bin of=$(obj)/disk.img bs=512 seek=8 conv=notrunc
	$(Q)$(DD) if=$(obj)/kernel.bin of=$(obj)/disk.img bs=512 seek=64 conv=notrunc
endif

clean-y += preload.bin preload-0x00000.bin disk.img

# run & install
ckfile := $(obj)/boot.bin
ifneq ($(ckfile), $(wildcard $(ckfile)))
run install: FORCE
	$(Q)$(ECHO) "Please run make first"
else
run: FORCE
	$(Q)$(SHELL) $(src)/run/$(run_shell)
install: FORCE
	$(Q)$(SHELL) $(src)/install
endif

uninsall:

clean-subdir-y += kboot/
clean-subdir-y += preload/
