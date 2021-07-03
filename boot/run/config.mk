#############################################################
# Run Config
#############################################################

ifdef CONFIG_CHIP_ESP32
run_shell       := esp32.sh
endif

ifdef CONFIG_CHIP_ESP8266
run_shell       := esp8266.sh
endif

ifdef CONFIG_CHIP_GX6605S
run_shell       := gx6605s.sh
endif

ifdef CONFIG_CHIP_STM32F0XX
run_shell       := stm32.sh
endif

ifdef CONFIG_CHIP_STM32F1XX
run_shell       := stm32.sh
endif

ifdef CONFIG_CHIP_STM32F4XX
run_shell       := stm32.sh
endif

ifdef CONFIG_CHIP_SUNXIF1
run_shell       := sunxif1.sh
endif

ifdef CONFIG_CHIP_SUNXIH3
run_shell       := sunxih3.sh
endif

ifdef CONFIG_CHIP_X86
run_shell       := qemu-x86.sh
endif
