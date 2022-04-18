# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += fdt.o early.o base.o
obj-y += iterator.o parser.o
obj-y += address.o bus.o irq.o
obj-$(CONFIG_BUILTIN_DTB) += dtb.o

quiet_cmd_mkincbin = $(ECHO_INCBIN) $@
      cmd_mkincbin = $(objtree)/tools/mkincbin/mkincbin .init.dtb $< > $@
$(obj)/dtb.S: $(subst ",,$(objtree)/$(CONFIG_BUILTIN_DTB_PATH)) FORCE
	$(call if_changed,mkincbin)

targets += dtb.S
clean-y += dtb.S
