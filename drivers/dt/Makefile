# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += address.o base.o fdt.o bus.o early.o
obj-$(CONFIG_BUILTIN_DTB) += dtb.o

quiet_cmd_mkincbin = MKINCBIN $@
      cmd_mkincbin = $(objtree)/tools/mkincbin/mkincbin .init.dtb $< > $@
$(obj)/dtb.S: $(subst ",,$(objtree)/$(CONFIG_BUILTIN_DTB_PATH)) FORCE
	$(call if_changed,mkincbin)
targets += dtb.S $(CONFIG_BUILTIN_DTB_PATH)
