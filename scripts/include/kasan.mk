# SPDX-License-Identifier: GPL-2.0-or-later

ifdef CONFIG_KASAN_GENERIC

ifdef CONFIG_KASAN_INLINE
call_threshold := 10000
else
call_threshold := 0
endif

KASAN_MINIMAL := -fsanitize=kernel-address
KASAN_SHADOW := $(call cc-option, -fsanitize=kernel-address \
                -fasan-shadow-offset=$(KASAN_SHADOW_OFFSET), \
                $(call cc-option, -fsanitize=kernel-address \
                -mllvm -asan-mapping-offset=$(KASAN_SHADOW_OFFSET)))

ifeq ($(strip $(KASAN_SHADOW)),)
CFLAGS_KASAN := $(KASAN_MINIMAL)
else
CFLAGS_KASAN := $(KASAN_SHADOW) \
                $(call cc-param,asan-globals=1) \
                $(call cc-param,asan-instrumentation-with-call-threshold=$(call_threshold)) \
                $(call cc-param,asan-instrument-allocas=1)
endif

endif # CONFIG_KASAN_GENERIC
