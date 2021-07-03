include-y := $(empty)
asflags-y := $(empty)
acflags-y := $(empty)
ccflags-y := $(empty)
ldflags-y := $(empty)

apps-y += ksh klib
apps-y := $(addprefix usr-, $(apps-y))
acflags-y += -Wall -Werror -ffreestanding -static
asflags-y += -D__ASSEMBLY__
include-y += $(src)/include/
include-y += $(src)/uapi/
include-y += $(src)/uapi/asm/

ifdef CONFIG_USR_XIP

rambase := $(CONFIG_USR_RAM_BASE)
xipbase := $(CONFIG_USR_XIP_BASE)
xipflags-y := -DRAM_BASE=$(CONFIG_USR_RAM_BASE) \
              -DXIP_BASE=$(CONFIG_USR_XIP_BASE)
acflags-y += -fPIE
$(apps-y): FORCE
	$(Q)$(MAKE) $(build)=$(src)/$@
	$(shell $(NM) $(obj)/kboot/boot | awk '$$NF == "_ld_text_end" {print $$1}')
	$(shell $(NM) $(obj)/kboot/boot | awk '$$NF == "_ld_bss_end" {print $$1}')

else #CONFIG_USR_XIP
$(apps-y): FORCE
	$(Q)$(MAKE) $(build)=$(src)/$(patsubst usr-%,%,$@)
endif

$(apps-y): link-asm
link-asm: FORCE
	$(Q)$(LN) ../../../../$(arch-include-y)/lightcore/asm \
	$(src)/uapi/asm/lightcore/

rootfs    := $(obj)/rootfs
rootdir-y := bin/ boot/ dev/ etc/ lib/ mnt/ root/ sbin/ sys/
rootdir   := $(addprefix $(rootfs)/,$(rootdir-y))
rootexc-y := . Makefile
rootexc   := $(addprefix -not -name ,$(rootexc-y))
export    rootfs

usr-genrootfs: $(apps-y)
	$(Q)$(MKDIR) $(rootdir)
	$(Q)$(CD) $(obj)/rootfs/;\
	$(FIND) . $(rootexc) | \
	$(MKCPIO) > ../rootfs.cpio

clean-y += $(addprefix rootfs/,$(rootdir-y)) rootfs.cpio
clean-subdir-y += $(patsubst usr-%,%/,$(apps-y))
