obj-y += address.o base.o fdt.o bus.o
obj-y += early.o

dtb := $(addprefix _dtb_,$(wildcard $(obj)/dtb.o))
$(dtb):
	$(Q)$(RM) $(patsubst _dtb_%,%,$@)
$(obj)/dtb.o: $(dtb)
obj-$(CONFIG_BUILTIN_DTB) += dtb.o
