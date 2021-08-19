#############################################################
# Platform Config
#############################################################
ifdef CONFIG_ARCH_X86
platform-asflags-y := $(empty)
platform-ccflags-y := $(empty)
platform-acflags-y := $(empty)
platform-ldflags-y := $(empty)
platform-ldsflags-y := $(empty)
platform-elfflags-y := $(empty)
endif

ifndef CONFIG_ARCH_X86
preload-lib += -lgcc
endif

ifdef CONFIG_ARCH_X86
include-y               += $(src)/bios/include/
platform-acflags-y      := -m16
platform-ldflags-y      := -m elf_i386
platform-elfflags-y     := -m16 -nostdlib -T $(src)/bios/boot.lds
endif 

ifdef CONFIG_CHIP_SUNIV
include-y               += $(src)/suniv/include/
platform-elfflags-y     := -nostdlib -T $(src)/suniv/boot.lds
endif 

ifdef CONFIG_CHIP_ESP8266
include-y               += $(src)/esp8266/include/
platform-elfflags-y     := -nostdlib -T $(src)/esp8266/boot.lds
endif 

ifdef CONFIG_CHIP_ESP32
include-y               += $(src)/esp32/include/
platform-elfflags-y     := -nostdlib -T $(src)/esp32/boot.lds
endif 
