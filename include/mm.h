/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_H_
#define _MM_H_

#include <types.h>
#include <stddef.h>
#include <mm/region.h>
#include <mm/page.h>
#include <mm/kmem.h>
#include <mm/vmem.h>
#include <mm/ioremap.h>
#include <mm/memblock.h>

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

#define MEM_MAGIC 0xDEADBEEF

#define page_align(addr) align_high(addr, PAGE_SIZE)
#define page_aligned(addr) aligned(addr, PAGE_SIZE)


void mem_init(void);

#endif /* _MM_H_ */
