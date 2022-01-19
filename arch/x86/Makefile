# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += boot/ lib/ mm/
obj-y += setup.o entry.o proc.o syscall.o
obj-y += idt.o irq.o backtrace.o traps.o
obj-y += doublefault.o e820.o tsc.o
obj-$(CONFIG_PRE_CONSOLE) += pre-console.o

obj-$(CONFIG_SMP) += smp.o
subdir-$(CONFIG_SMP) += smpboot/
$(obj)/smp.o: $(obj)/smpboot/

$(obj)/setup.o: $(obj)/asm-offset.h
$(obj)/lib: $(obj)/asm-offset.h

extra-y += asm-offset.h kernel.lds
clean-y += include/generated/
