# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
#

include $(build_home)/platform.mk
include $(build_home)/compile.mk

#####################################
# Subproject                        #
#####################################

obj-y += doc/logo/
obj-y += doc/translate/

obj-y += arch/
obj-y += drivers/
obj-y += fs/
obj-y += init/
obj-y += ipc/
obj-y += kernel/
obj-y += lib/
obj-y += mm/
obj-y += net/
obj-$(CONFIG_KUSER) += usr/

#####################################
# Generate romdisk                  #
#####################################

romdisk_files := $(shell $(FIND) $(obj)/boot/romdisk/)
$(romdisk_files): $(obj)/doc/translate

$(obj)/boot/romdisk.cpio: $(romdisk_files) FORCE
	$(call if_changed,cpio)

ifdef CONFIG_ROMDISK
$(obj)/drivers: $(obj)/boot/romdisk.cpio FORCE
targets += boot/romdisk.cpio
endif

lightcore-objs += built-in.o
elf-always-y += lightcore

lightcore.dump-obj += lightcore
dump-always-$(CONFIG_KERNEL_DUMP) += lightcore.dump

clean-subdir-y += boot/
