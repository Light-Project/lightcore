/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_H_
#define _MM_H_

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
extern char _ld_init_dtb_start;
extern char _ld_init_dtb_end;
extern char _ld_image_end;

#define page_align(addr)    align_high(addr, PAGE_SIZE)
#define page_aligned(addr)  align_check(addr, PAGE_SIZE)

#define IMAGE_SIZE page_align(va_to_pa(&_ld_image_end) - NORMAL_OFFSET)

void mem_init(void);

#endif  /* _MM_H_ */
