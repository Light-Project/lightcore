# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# Xtensa Platform configure
# ==========================================================================

ifdef CONFIG_XTENSA_VARIANT_LX106
CROSS_COMPILE       ?= xtensa-lx106-elf-
include-y           += arch/xtensa/variants/lx106/include/
endif
