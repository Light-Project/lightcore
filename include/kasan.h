/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KASAN_H_
#define _KASAN_H_

#include <kernel.h>
#include <asm/kasan.h>

#define KASAN_GRANULE_MASK 0

static inline void *kasan_to_shadow(const void *addr)
{
    return (void *)((unsigned long)addr >>
        KASAN_SHADOW_SHIFT) + KASAN_SHADOW_OFFSET;
}

extern void __asan_load1(unsigned long addr);
extern void __asan_load2(unsigned long addr);
extern void __asan_load4(unsigned long addr);
extern void __asan_load8(unsigned long addr);
extern void __asan_load16(unsigned long addr);
extern void __asan_store1(unsigned long addr);
extern void __asan_store2(unsigned long addr);
extern void __asan_store4(unsigned long addr);
extern void __asan_store8(unsigned long addr);
extern void __asan_store16(unsigned long addr);
extern void __asan_loadN(unsigned long addr, size_t size);
extern void __asan_storeN(unsigned long addr, size_t size);

extern void __asan_load1_noabort(unsigned long addr);
extern void __asan_load2_noabort(unsigned long addr);
extern void __asan_load4_noabort(unsigned long addr);
extern void __asan_load8_noabort(unsigned long addr);
extern void __asan_store1_noabort(unsigned long addr);
extern void __asan_store2_noabort(unsigned long addr);
extern void __asan_store4_noabort(unsigned long addr);
extern void __asan_store8_noabort(unsigned long addr);
extern void __asan_load16_noabort(unsigned long addr);
extern void __asan_store16_noabort(unsigned long addr);
extern void __asan_loadN_noabort(unsigned long addr, size_t size);
extern void __asan_storeN_noabort(unsigned long addr, size_t size);

extern void __asan_report_load1_noabort(unsigned long addr);
extern void __asan_report_load2_noabort(unsigned long addr);
extern void __asan_report_load4_noabort(unsigned long addr);
extern void __asan_report_load8_noabort(unsigned long addr);
extern void __asan_report_load16_noabort(unsigned long addr);
extern void __asan_report_store1_noabort(unsigned long addr);
extern void __asan_report_store2_noabort(unsigned long addr);
extern void __asan_report_store4_noabort(unsigned long addr);
extern void __asan_report_store8_noabort(unsigned long addr);
extern void __asan_report_store16_noabort(unsigned long addr);
extern void __asan_report_load_n_noabort(unsigned long addr, size_t size);
extern void __asan_report_store_n_noabort(unsigned long addr, size_t size);

extern void __asan_set_shadow_00(const void *addr, size_t size);
extern void __asan_set_shadow_f1(const void *addr, size_t size);
extern void __asan_set_shadow_f2(const void *addr, size_t size);
extern void __asan_set_shadow_f3(const void *addr, size_t size);
extern void __asan_set_shadow_f5(const void *addr, size_t size);
extern void __asan_set_shadow_f8(const void *addr, size_t size);

/* kasan core */
extern bool kasan_report(unsigned long addr, size_t size, bool is_write, unsigned long ip);

#endif  /* _KASAN_H_ */
