# SPDX-License-Identifier: GPL-2.0-or-later
UBSAN_SANITIZE_ubsan.o := n
obj-y += device/ lock/ irq/ sched/
obj-y += printk/ binfmt/ time/
obj-y += namespace/ task/
obj-y += kthread.o kcoro.o entry.o
obj-y += notifier.o pevent.o
obj-y += crash.o extable.o panic.o
obj-y += syscall.o random.o
obj-y += resource.o power.o
obj-$(CONFIG_UBSAN) += ubsan.o
obj-$(CONFIG_KGCOV) += kgcov/
obj-$(CONFIG_GENERIC_MPIO) += mpio.o
