# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += header.o startup.o memdetect.o a20.o
obj-$(CONFIG_ARCH_X86_32) += startup32.o
obj-$(CONFIG_ARCH_X86_64) += startup64.o
a20-flags-y := -m16 -fno-pic
memdetect-flags-y := -m16 -fno-pic
