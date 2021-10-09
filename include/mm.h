/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_H_
#define _MM_H_

#include <kernel.h>
#include <mm/memmodel.h>
#include <kmalloc.h>
#include <mm/vmem.h>

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

#define MEM_MAGIC 0xDEADBEEF

#define page_align(addr) align_high(addr, PAGE_SIZE)
#define page_aligned(addr) aligned(addr, PAGE_SIZE)

#define va_to_pa(va)        ((phys_addr_t)(va) - CONFIG_PAGE_OFFSET + CONFIG_RAM_BASE)
#define pa_to_va(pa)        ((void *)((pa) - CONFIG_RAM_BASE + CONFIG_PAGE_OFFSET))

#define page_to_pa(page)    (page_to_nr(page) << PAGE_SHIFT)
#define pa_to_page(pa)      (nr_to_page(pa >> PAGE_SHIFT))

#define page_to_va(page)    (pa_to_va(page_to_pa(page)))
#define va_to_page(address) (pa_to_page(va_to_pa(address)))

void mem_init(void);

#endif /* _MM_H_ */
