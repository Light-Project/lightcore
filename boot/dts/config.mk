# SPDX-License-Identifier: GPL-2.0-or-later

ifdef CONFIG_ARCH_CSKY
dtb-y += gx6605s.dtb
endif

ifdef CONFIG_ARCH_X86
dtb-y += ibmpc.dtb
endif

ifdef CONFIG_ARCH_XTENSA
dtb-y += esp32.dtb
dtb-y += esp8266.dtb
endif 
