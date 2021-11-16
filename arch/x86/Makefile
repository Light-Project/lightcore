# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += boot/ lib/ mm/
obj-y += setup.o entry.o proc.o syscall.o
obj-y += idt.o irq.o backtrace.o traps.o
obj-y += e820.o tsc.o pre_printk.o

obj-y += doublefault.o

obj-$(CONFIG_SMP) += smp.o
subdir-$(CONFIG_SMP) += smpboot/
$(obj)/smp.o: $(obj)/smpboot/

extra-y += kernel.lds
clean-y += include/generated/
