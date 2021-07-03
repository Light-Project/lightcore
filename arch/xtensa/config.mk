ifdef CONFIG_XTENSA_VARIANT_LX106
CROSS_COMPILE       := xtensa-lx106-elf-
include-y           += arch/xtensa/variants/lx106/include/
endif

ifdef CONFIG_XTENSA_VARIANT_ESP32
CROSS_COMPILE       := xtensa-esp32-elf-
include-y           += arch/xtensa/variants/esp32/include/
endif
