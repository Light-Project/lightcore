# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += header.o startup.o main.o page.o segment.o video.o memdetect.o
obj-$(CONFIG_KASLR) += kaslr.o

memdetect.o-flags-y := -m16
