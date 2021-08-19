tar-$(CONFIG_ARCH_ARM)      += arm/
tar-$(CONFIG_ARCH_CSKY)     += csky/
tar-$(CONFIG_ARCH_RISCV64)  += riscv64/
tar-$(CONFIG_ARCH_X86)      += x86/
obj-y += $(tar-y)
obj-y += lib/

extra-y += kboot.lds

include-y += $(src)/$(tar-y)
include-y += $(src)/lib/

kboot-asflags-y := -D__ASSEMBLY__
kboot-ccflags-y := -std=gnu11
kboot-acflags-y := -ffreestanding -nostdinc -Wall -Werror \
                   -fPIE  -fno-stack-protector \
                   -ffunction-sections -fdata-sections 

kboot-ldsflags-y := -D__ASSEMBLY__

kboot-elfflags-y := -Wl,--gc-sections
kboot-elfflags-y += -e _start -Wl,--no-dynamic-linker -Wl,--build-id=none -fPIE

asflags-y   := $(strip $(kboot-asflags-y) $(platform-asflags-y))
ccflags-y   := $(strip $(kboot-ccflags-y) $(platform-ccflags-y))
acflags-y   := $(strip $(kboot-acflags-y) $(platform-acflags-y))
ldflags-y   := $(strip $(kboot-ldflags-y) $(platform-ldflags-y))
ldsflags-y  := $(strip $(kboot-ldsflags-y) $(platform-ldsflags-y))
elfflags-y  := $(strip $(kboot-elfflags-y) $(platform-eflags-y))

quiet_cmd_link_boot = $(ECHO_ELF)  $@ 
      cmd_link_boot = $(RANLIB) $(obj)/built-in.a; \
                      $(LD) $(ldflags-y) -r -o $(obj)/built-in.o --whole-archive $(obj)/built-in.a; \
                      $(MKELF) $(elfflags-y) -T $(src)/kboot.lds -o $@ $(obj)/built-in.o -lgcc
$(obj)/boot: $(obj)/built-in.a $(obj)/kboot.lds
	$(call if_changed,link_boot)
boot: $(obj)/boot

clean-y += boot boot.map built-in.o
clean-subdir-y += $(tar-)
