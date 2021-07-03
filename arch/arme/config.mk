CROSS_COMPILE := arm-none-eabi-
acflags-y += -march=armv7-m -mcpu=cortex-m3 -mthumb
MKELF := $(CROSS_COMPILE)gcc 
elf-flags-y += -nostdlib -march=armv7-m -mcpu=cortex-m3 -mthumb 
kernel-direct-y += -lgcc
export CROSS_COMPILE

