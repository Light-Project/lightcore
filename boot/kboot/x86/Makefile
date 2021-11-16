# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += header.o startup.o memdetect.o
obj-y += main.o page.o segment.o video.o
obj-$(CONFIG_KASLR) += kaslr.o

memdetect-flags-y := -m16 -fno-pic
