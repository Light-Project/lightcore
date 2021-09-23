# SPDX-License-Identifier: GPL-2.0-or-later

arch-$(CONFIG_ARCH_ARM)     += arm/
arch-$(CONFIG_ARCH_CSKY)    += csky/
arch-$(CONFIG_ARCH_RISCV64) += riscv64/
arch-$(CONFIG_ARCH_X86)     += x86/
obj-y += $(arch-y) lib/ piggy.o
extra-y += kboot.lds

include-y += $(arch-y) lib/

kboot-asflags-y  += -D__ASSEMBLY__
kboot-ccflags-y  += -std=gnu11
kboot-acflags-y  += -ffreestanding -nostdinc -Wall -Werror -fPIE -fno-stack-protector -fno-common
kboot-ldsflags-y += -D__ASSEMBLY__
kboot-elfflags-y += -T $(obj)/kboot.lds -Wl,--cref,-Map=$@.map
kboot-elfflags-y += -Wl,--no-dynamic-linker -nostdlib -lgcc

asflags-y   = $(strip $(kboot-asflags-y) $(platform-asflags-y))
ccflags-y   = $(strip $(kboot-ccflags-y) $(platform-ccflags-y))
acflags-y   = $(strip $(kboot-acflags-y) $(platform-acflags-y))
ldflags-y   = $(strip $(kboot-ldflags-y) $(platform-ldflags-y))
ldsflags-y  = $(strip $(kboot-ldsflags-y) $(platform-ldsflags-y))
kboot-flags-y = $(strip $(kboot-elfflags-y) $(platform-elfflags-y))

kboot-objs += built-in.o
elf-always-y += kboot

boot.bin-obj += kboot
bin-always-y += boot.bin

fixtool: FORCE
	$(Q)$(MAKE) $(build)=tools/kernelcrc

quiet_cmd_copy_kernel = $(ECHO_BIN) $@
      cmd_copy_kernel = $(OBJCOPY) -O binary $< $@
quiet_cmd_fix_kernel  = FIXCRC $@
      cmd_fix_kernel  = tools/kernelcrc/kernelcrc $@
$(obj)/kernel.bin: lightcore
	$(call if_changed,copy_kernel)
	$(call if_changed,fix_kernel)

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

quiet_cmd_mkpiggy = MKPIGGY $@
      cmd_mkpiggy = $(objtree)/tools/mkpiggy/mkpiggy $< > $@
$(obj)/piggy.S: $(obj)/kernel.bin$(suffix-y) FORCE
	$(call if_changed,mkpiggy)
targets += piggy.S kernel.bin$(suffix-y)

clean-suffix += .gz .bz2 .lzma .xz .lzo .lz4 .zst
clean-y += $(addprefix kernel.bin,$(clean-suffix))
clean-y += kernel.bin piggy.S
