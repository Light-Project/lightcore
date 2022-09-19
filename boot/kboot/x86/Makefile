# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += boot/ realmode/
obj-y += main.o segment.o
obj-$(CONFIG_ARCH_X86_32) += page32.o
obj-$(CONFIG_ARCH_X86_64) += page64.o
obj-$(CONFIG_KBOOT_CON_VGA) += video.o
obj-$(CONFIG_KBOOT_CON_SER) += serial.o
