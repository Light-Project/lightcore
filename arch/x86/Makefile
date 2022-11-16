# SPDX-License-Identifier: GPL-2.0-or-later
obj-y += boot/ lib/ mm/
obj-y += setup.o nmi.o
obj-y += idt.o irq.o traps.o extable.o
obj-y += e820.o tsc.o syscall.o
obj-y += earlycon-vga.o earlycon-serial.o

obj-$(CONFIG_ARCH_X86_32) += entry32.o
obj-$(CONFIG_ARCH_X86_32) += doublefault32.o
obj-$(CONFIG_ARCH_X86_32) += backtrace32.o
obj-$(CONFIG_ARCH_X86_32) += proc32.o

obj-$(CONFIG_ARCH_X86_64) += entry64.o
obj-$(CONFIG_ARCH_X86_64) += backtrace64.o
obj-$(CONFIG_ARCH_X86_64) += proc64.o

obj-$(CONFIG_SMP) += smp.o
subdir-$(CONFIG_SMP) += smpboot/
$(obj)/smp.o: $(obj)/smpboot/

$(obj)/setup.o: $(obj)/asm-offset.h
$(obj)/entry32.o: $(obj)/asm-offset.h
$(obj)/lib: $(obj)/asm-offset.h

extra-y += asm-offset.h kernel.lds
clean-y += include/generated/
