/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kasan.h>
#include <string.h>
#include <export.h>

static __always_inline unsigned long
mem_is_nonzero(const uint8_t *start, size_t size)
{
    while (size) {
        if (unlikely(*start))
            return (unsigned long)start;
        start++;
        size--;
    }

    return 0;
}

static __always_inline bool
memory_poisoned_1(unsigned long addr, size_t size)
{
    return false;
}

static __always_inline bool
memory_poisoned_2_4_8(unsigned long addr, size_t size)
{
    return false;
}

static __always_inline bool
memory_poisoned_16(unsigned long addr, size_t size)
{
    return false;
}

static __always_inline bool
memory_poisoned_n(unsigned long addr, size_t size)
{
    return false;
}

static __always_inline bool
memory_poisoned(unsigned long addr, size_t size)
{
    if (__builtin_constant_p(size)) {
        switch (size) {
            case 1: default:
                return memory_poisoned_1(addr, size);

            case 2: case 4: case 8:
                return memory_poisoned_2_4_8(addr, size);

            case 16:
                return memory_poisoned_16(addr, size);
        }
    }

    return memory_poisoned_n(addr, size);
}

static __always_inline void
asan_check_region_inline(unsigned long addr, size_t size, bool write, unsigned long rip)
{
    if (size == 0)
        return;

    if (unlikely(addr + size < addr)) {
        kasan_report(addr, size, write, rip);
        return;
    }

    if (memory_poisoned(addr, size)) {
        kasan_report(addr, size, write, rip);
        return;
    }
}

static void asan_check_region(unsigned long addr, size_t size, bool write, unsigned long rip)
{
    asan_check_region_inline(addr, size, write, rip);
}

#define GENERIC_ASAN_LOAD(size)                             \
void __asan_load##size(unsigned long add)                   \
{                                                           \
    asan_check_region_inline(add, size, false, _RET_IP_);   \
}                                                           \
__alias(__asan_load##size)                                  \
void __asan_load##size##_noabort(unsigned long);

#define GENERIC_ASAN_STORE(size)                            \
void __asan_store##size(unsigned long add)                  \
{                                                           \
    asan_check_region_inline(add, size, true, _RET_IP_);    \
}                                                           \
__alias(__asan_store##size)                                 \
void __asan_store##size##_noabort(unsigned long);

GENERIC_ASAN_LOAD(1)
GENERIC_ASAN_LOAD(2)
GENERIC_ASAN_LOAD(4)
GENERIC_ASAN_LOAD(8)
GENERIC_ASAN_LOAD(16)
GENERIC_ASAN_STORE(1)
GENERIC_ASAN_STORE(2)
GENERIC_ASAN_STORE(4)
GENERIC_ASAN_STORE(8)
GENERIC_ASAN_STORE(16)

EXPORT_SYMBOL(__asan_load1);
EXPORT_SYMBOL(__asan_load2);
EXPORT_SYMBOL(__asan_load4);
EXPORT_SYMBOL(__asan_load8);
EXPORT_SYMBOL(__asan_load16);
EXPORT_SYMBOL(__asan_store1);
EXPORT_SYMBOL(__asan_store2);
EXPORT_SYMBOL(__asan_store4);
EXPORT_SYMBOL(__asan_store8);
EXPORT_SYMBOL(__asan_store16);

EXPORT_SYMBOL(__asan_load1_noabort);
EXPORT_SYMBOL(__asan_load2_noabort);
EXPORT_SYMBOL(__asan_load4_noabort);
EXPORT_SYMBOL(__asan_load8_noabort);
EXPORT_SYMBOL(__asan_load16_noabort);
EXPORT_SYMBOL(__asan_store1_noabort);
EXPORT_SYMBOL(__asan_store2_noabort);
EXPORT_SYMBOL(__asan_store4_noabort);
EXPORT_SYMBOL(__asan_store8_noabort);
EXPORT_SYMBOL(__asan_store16_noabort);

void __asan_loadN(unsigned long addr, size_t size)
{
    asan_check_region(addr, size, false, _RET_IP_);
}
EXPORT_SYMBOL(__asan_loadN);

__alias(__asan_loadN)
void __asan_loadN_noabort(unsigned long, size_t);
EXPORT_SYMBOL(__asan_loadN_noabort);

void __asan_storeN(unsigned long addr, size_t size)
{
    asan_check_region(addr, size, true, _RET_IP_);
}
EXPORT_SYMBOL(__asan_storeN);

__alias(__asan_storeN)
void __asan_storeN_noabort(unsigned long, size_t);
EXPORT_SYMBOL(__asan_storeN_noabort);

void __asan_handle_no_return(void) {}
EXPORT_SYMBOL(__asan_handle_no_return);

#define GENERIC_ASAN_SET_SHADOW(byte)                           \
void __asan_set_shadow_##byte(const void *addr, size_t size)    \
{                                                               \
    memset((void *)addr, 0x##byte, size);                       \
}

GENERIC_ASAN_SET_SHADOW(00);
GENERIC_ASAN_SET_SHADOW(f1);
GENERIC_ASAN_SET_SHADOW(f2);
GENERIC_ASAN_SET_SHADOW(f3);
GENERIC_ASAN_SET_SHADOW(f5);
GENERIC_ASAN_SET_SHADOW(f8);

EXPORT_SYMBOL(__asan_set_shadow_00);
EXPORT_SYMBOL(__asan_set_shadow_f1);
EXPORT_SYMBOL(__asan_set_shadow_f2);
EXPORT_SYMBOL(__asan_set_shadow_f3);
EXPORT_SYMBOL(__asan_set_shadow_f5);
EXPORT_SYMBOL(__asan_set_shadow_f8);
