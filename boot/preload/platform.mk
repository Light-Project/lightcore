#############################################################
# Platform Config
#############################################################

ifdef CONFIG_ARCH_X86
include-y               += bios/include/
platform-acflags-y      := -m16
platform-asflags-y      := $(empty)
platform-ccflags-y      := -mno-sse -mno-mmx -mno-sse2
platform-ccflags-y      += -mno-3dnow -mno-avx
platform-ldflags-y      := -m elf_i386
platform-ldsflags-y     := $(empty)
platform-elfflags-y     := -m elf_i386 -T $(src)/bios/boot.lds
platform-ccflags-$(CONFIG_ARCH_X86_64) += -D CONFIG_ARCH_X86_32
endif

ifdef CONFIG_CHIP_SUNIV
include-y               += suniv/include/
platform-elfflags-y     := -T $(src)/suniv/boot.lds
endif

ifdef CONFIG_CHIP_ESP8266
include-y               += esp8266/include/
platform-ccflags-y      += -mtext-section-literals
platform-acflags-y      += -pipe -mlongcalls
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
