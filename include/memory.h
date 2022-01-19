/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <mm/page.h>
#include <mm/memmodel.h>

extern char _ld_startup_start;
extern char _ld_startup_end;
extern char _ld_text_start;
extern char _ld_text_end;
extern char _ld_rodata_rodata_start;
extern char _ld_rodata_rodata_end;
extern char _ld_data_section_start;
extern char _ld_data_section_end;
extern char _ld_bss_start;
extern char _ld_bss_end;
extern char _ld_image_end;

#ifdef CONFIG_BUILTIN_DTB
extern char _ld_init_dtb_start;
extern char _ld_init_dtb_end;
#endif

#ifdef CONFIG_ROMDISK
extern char _ld_romdisk_start;
extern char _ld_romdisk_end;
#endif

#define page_align(addr) align_high(addr, PAGE_SIZE)
#define page_aligned(addr) align_check(addr, PAGE_SIZE)
#define IMAGE_SIZE page_align(va_to_pa(&_ld_image_end) - NORMAL_OFFSET)

static inline void memory_get(struct memory *mem)
{
    refcount_inc(&mem->users);
}

static inline bool memory_put(struct memory *mem)
{
    return refcount_dec_test(&mem->users);
}

extern void __init mem_init(void);

#endif  /* _MEMORY_H_ */
