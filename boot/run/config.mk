#############################################################
# Run Config
#############################################################

ifdef CONFIG_CHIP_ESP8266
run_shell := esp8266.sh
endif

ifdef CONFIG_CHIP_GX6605S
run_shell := gx6605s.sh
endif

ifdef CONFIG_CHIP_SUNIV
run_shell := suniv.sh
endif

ifdef CONFIG_CHIP_X86
run_shell := qemu-x86.sh
endif
