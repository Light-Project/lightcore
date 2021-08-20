CROSS_COMPILE := riscv64-linux-gnu-
acflags-y := -march=rv64imafdc -mabi=lp64d -mcmodel=medany
export CROSS_COMPILE
MKELF := $(CROSS_COMPILE)gcc
elf-flags-y += -nostdlib -T $(obj)arch/riscv64/kernel.lds
kernel-direct-y += -lgcc
