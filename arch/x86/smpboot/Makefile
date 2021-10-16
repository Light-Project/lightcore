# SPDX-License-Identifier: GPL-2.0-or-later
asflags-y   = -D__ASSEMBLY__
ccflags-y   = -std=gnu11
acflags-y   = -m16 -ffreestanding -nostdinc -fno-pic -fno-stack-protector
ldflags-y   = -m elf_i386
ldsflags-y  = -D__ASSEMBLY__
smpboot-flags-y = -T $(src)/smpboot.lds -nostdlib -Map=$@.map --emit-relocs

obj-y += smpboot.o
extra-y += smpboot.lds

smpboot-objs += built-in.o
elf-always-y += smpboot
