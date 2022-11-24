# SPDX-License-Identifier: GPL-2.0-or-later
platform-acflags-$(CONFIG_ARCH_X86_64) += -mcmodel=small
platform-elfflags-y += --no-check-sections
