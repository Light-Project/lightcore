#############################################################
# Platform Config
#############################################################
ifndef CONFIG_ARCH_X86
preload-lib += -lgcc
endif

ifdef CONFIG_ARCH_X86
platform-asflags-y := $(empty)
platform-ccflags-y := $(empty)
platform-acflags-y := $(empty)
platform-ldsflags-y := $(empty)
platform-elfflags-y := $(empty)
endif

ifdef CONFIG_ARCH_X86
include-y               += bios/include/
platform-acflags-y      := -m16
platform-ldflags-y      := -m elf_i386
platform-elfflags-y     := -Wl,-m elf_i386 -Wl,-T $(src)/bios/boot.lds
endif

ifdef CONFIG_CHIP_SUNIV
include-y               += suniv/include/
platform-elfflags-y     := -T $(src)/suniv/boot.lds
endif

ifdef CONFIG_CHIP_RK3399
include-y               += rk3399/include/
platform-elfflags-y     := -T $(src)/rk3399/boot.lds
endif

ifdef CONFIG_CHIP_ESP8266
include-y               += esp8266/include/
platform-elfflags-y     := -T $(src)/esp8266/boot.lds
endif

ifdef CONFIG_CHIP_ESP32
include-y               += esp32/include/
platform-elfflags-y     := -T $(src)/esp32/boot.lds
endif

ifdef CONFIG_CHIP_GX6605S
include-y               += gx6605s/include/
platform-acflags-y      := -mlittle-endian -mcpu=ck610 -Os
platform-ldflags-y      := -EL
platform-elfflags-y     := -T $(src)/gx6605s/boot.lds
endif
