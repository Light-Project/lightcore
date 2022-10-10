/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kasan.h>
#include <string.h>
#include <export.h>

static __always_inline bool
memory_poisoned_1(uintptr_t addr)
{
    int8_t shadow_value = *(int8_t *)kasan_to_shadow((void *)addr);

    if (unlikely(shadow_value)) {
        int8_t last_accessible_byte = addr & KASAN_GRANULE_MASK;
        return unlikely(last_accessible_byte >= shadow_value);
    }

    return false;
}

static __always_inline bool
memory_poisoned_2_4_8(uintptr_t addr, size_t size)
{
    uint8_t *shadow_addr = (uint8_t *)kasan_to_shadow((void *)addr);

    if (unlikely(((addr + size - 1) & KASAN_GRANULE_MASK) < size - 1))
        return *shadow_addr || memory_poisoned_1(addr + size - 1);

    return memory_poisoned_1(addr + size - 1);
}

static __always_inline bool
memory_poisoned_16(uintptr_t addr)
{
    return false;
}

static __always_inline uintptr_t
bytes_is_nonzero(const uint8_t *start, size_t size)
{
    while (size--) {
        if (unlikely(*start))
            return (uintptr_t)start;
        start++;
    }

    return 0;
}

static __always_inline uintptr_t
memory_is_nonzero(const void *start, const void *end)
{
    unsigned int prefix = (uintptr_t)start % 8;
    unsigned int words, ret;

    if (end - start <= 16)
        return bytes_is_nonzero(start, end - start);

    if (prefix) {
        prefix = 8 - prefix;
        ret = bytes_is_nonzero(start, prefix);
        if (unlikely(ret))
            return ret;
        start += prefix;
    }

    words = (end - start) / 8;
    while (words--) {
        if (unlikely(*(u64 *)start))
            return bytes_is_nonzero(start, 8);
        start += 8;
    }

    return bytes_is_nonzero(start, (end - start) % 8);
}

static __always_inline bool
memory_poisoned_n(uintptr_t addr, size_t size)
{
    uintptr_t ret;

    ret = memory_is_nonzero(
        kasan_to_shadow((void *)addr),
        kasan_to_shadow((void *)addr + size - 1) + 1
    );

    if (unlikely(ret)) {
        uintptr_t last_byte = addr + size - 1;
        int8_t *last_shadow = (int8_t *)kasan_to_shadow((void *)last_byte);

        if (unlikely(ret != (uintptr_t)last_shadow ||
            ((long)(last_byte & KASAN_GRANULE_MASK) >= *last_shadow)))
            return true;
    }

    return false;
}

static __always_inline bool
memory_poisoned(uintptr_t addr, size_t size)
{
    if (__builtin_constant_p(size)) {
        switch (size) {
            case 1: default:
                return memory_poisoned_1(addr);

            case 2: case 4: case 8:
                return memory_poisoned_2_4_8(addr, size);

            case 16:
                return memory_poisoned_16(addr);
        }
    }

    return memory_poisoned_n(addr, size);
}

static __always_inline bool
kasan_check_region_inline(uintptr_t addr, size_t size, bool write, uintptr_t rip)
{
    if (unlikely(size == 0))
        return true;

    if (unlikely(addr + size < addr))
        return kasan_report(addr, size, write, rip);

    if (memory_poisoned(addr, size))
        return kasan_report(addr, size, write, rip);

    return true;
}

bool kasan_check_region(uintptr_t addr, size_t size, bool write, uintptr_t rip)
{
    return kasan_check_region_inline(addr, size, write, rip);
}
EXPORT_SYMBOL(kasan_check_region);

#define GENERIC_ASAN_LOAD(size)                             \
void __asan_load##size(uintptr_t add)                       \
{                                                           \
    kasan_check_region_inline(add, size, false, _RET_IP_);  \
}                                                           \
__alias(__asan_load##size)                                  \
void __asan_load##size##_noabort(uintptr_t);

#define GENERIC_ASAN_STORE(size)                            \
void __asan_store##size(uintptr_t add)                      \
{                                                           \
    kasan_check_region_inline(add, size, true, _RET_IP_);   \
}                                                           \
__alias(__asan_store##size)                                 \
void __asan_store##size##_noabort(uintptr_t);

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

void __asan_loadN(uintptr_t addr, size_t size)
{
    kasan_check_region(addr, size, false, _RET_IP_);
}
EXPORT_SYMBOL(__asan_loadN);

__alias(__asan_loadN)
void __asan_loadN_noabort(uintptr_t, size_t);
EXPORT_SYMBOL(__asan_loadN_noabort);

void __asan_storeN(uintptr_t addr, size_t size)
{
    kasan_check_region(addr, size, true, _RET_IP_);
}
EXPORT_SYMBOL(__asan_storeN);

__alias(__asan_storeN)
void __asan_storeN_noabort(uintptr_t, size_t);
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
