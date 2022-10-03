/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SECTIONS_H_
#define _SECTIONS_H_

#include <types.h>

extern void *_ld_startup_start[];
extern void *_ld_startup_end[];
extern void *_ld_text_start[];
extern void *_ld_text_end[];
extern void *_ld_rodata_start[];
extern void *_ld_rodata_end[];
extern void *_ld_data_start[];
extern void *_ld_data_end[];
extern void *_ld_inittext_start[];
extern void *_ld_inittext_end[];
extern void *_ld_initdata_start[];
extern void *_ld_initdata_end[];
extern void *_ld_exittext_start[];
extern void *_ld_exittext_end[];
extern void *_ld_exitdata_start[];
extern void *_ld_exitdata_end[];
extern void *_ld_bss_start[];
extern void *_ld_bss_end[];

extern void *_ld_image_start[];
extern void *_ld_image_end[];
extern void *_ld_reserve_end[];

extern void *_ld_data_bug_table_start[];
extern void *_ld_data_bug_table_end[];
extern void *_ld_extable_start[];
extern void *_ld_extable_end[];

#ifdef CONFIG_BUILTIN_DTB
extern void *_ld_init_dtb_start[];
extern void *_ld_init_dtb_end[];
#endif

#ifdef CONFIG_ROMDISK
extern void *_ld_romdisk_start[];
extern void *_ld_romdisk_end[];
#endif

#define GENERIC_SECTION_FUNC(section)                       \
static inline bool in_##section##_section(const void *ptr)  \
{                                                           \
    return ptr >= (const void *)_ld_##section##_start &&    \
           ptr <  (const void *)_ld_##section##_end;        \
}

GENERIC_SECTION_FUNC(startup)
GENERIC_SECTION_FUNC(text)
GENERIC_SECTION_FUNC(rodata)
GENERIC_SECTION_FUNC(data)
GENERIC_SECTION_FUNC(inittext)
GENERIC_SECTION_FUNC(initdata)
GENERIC_SECTION_FUNC(exittext)
GENERIC_SECTION_FUNC(exitdata)
GENERIC_SECTION_FUNC(bss)

#define RESERVE_SIZE page_align((uintptr_t)_ld_reserve_end - (uintptr_t)_ld_image_start)
#define ROMDISK_SIZE ((uintptr_t)_ld_romdisk_end - (uintptr_t)_ld_romdisk_start)

#endif  /* _SECTIONS_H_ */
