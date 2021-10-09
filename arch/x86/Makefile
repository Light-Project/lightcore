obj-y += boot/ lib/ mm/
obj-y += entry.o
obj-y += setup.o
obj-y += proc.o
obj-y += syscall.o
obj-y += tsc.o

obj-y += irq.o
obj-y += idt.o
obj-y += backtrace.o
obj-y += traps.o

obj-$(CONFIG_SMP) += smp.o

obj-y += pre_printk.o

extra-y += kernel.lds
clean-y += include/generated/
