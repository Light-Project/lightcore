# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += romdisk.o data.o

quiet_cmd_mkincbin = $(ECHO_INCBIN) $@
      cmd_mkincbin = $(objtree)/tools/mkincbin/mkincbin .init.dtb $< > $@
$(obj)/data.S: $(objtree)/boot/romdisk.cpio FORCE
	$(call if_changed,mkincbin)
targets += data.S
