/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_H_
#define _MM_H_

#include <asm/page.h>

#if defined(CONFIG_REGION_DMA) && defined(CONFIG_REGION_DMA32)
# define NORMAL_OFFSET CONFIG_RAM_BASE + CONFIG_RAM_PAD + \
         CONFIG_DMA_SIZE + CONFIG_DMA32_SIZE
#elif defined(CONFIG_REGION_DMA)
# define NORMAL_OFFSET CONFIG_RAM_BASE + CONFIG_RAM_PAD + \
         CONFIG_DMA_SIZE
#else
# define NORMAL_OFFSET CONFIG_RAM_BASE + CONFIG_RAM_PAD
#endif

#ifdef __ASSEMBLY__
# define va_to_pa(va) ((va) - CONFIG_PAGE_OFFSET + CONFIG_RAM_BASE)
# define pa_to_va(pa) ((pa) - CONFIG_RAM_BASE + CONFIG_PAGE_OFFSET)
#else
# define va_to_pa(va) ((phys_addr_t)(va) - CONFIG_PAGE_OFFSET + CONFIG_RAM_BASE)
# define pa_to_va(pa) ((void *)((pa) - CONFIG_RAM_BASE + CONFIG_PAGE_OFFSET))
#endif

#ifndef __ASSEMBLY__

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

#define va_to_pa(va) ((phys_addr_t)(va) - CONFIG_PAGE_OFFSET + CONFIG_RAM_BASE)
#define pa_to_va(pa) ((void *)((pa) - CONFIG_RAM_BASE + CONFIG_PAGE_OFFSET))

#define page_to_pa(page)    (page_to_nr(page) << PAGE_SHIFT)
#define pa_to_page(pa)      (nr_to_page(pa >> PAGE_SHIFT))

#define page_to_va(page)    (pa_to_va(page_to_pa(page)))
#define va_to_page(address) (pa_to_page(va_to_pa(address)))

#define page_to_size(page)  (PAGE_SIZE << (page)->order)

void mem_init(void);

#endif /* __ASSEMBLY__ */
#endif /* _MM_H_ */
