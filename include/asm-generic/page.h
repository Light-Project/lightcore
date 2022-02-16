/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PAGE_H_
#define _ASM_GENERIC_PAGE_H_

#if defined(CONFIG_REGION_DMA)
# define NORMAL_OFFSET \
    (CONFIG_RAM_BASE + CONFIG_RAM_PAD + CONFIG_DMA_SIZE)
#else
# define NORMAL_OFFSET \
    (CONFIG_RAM_BASE + CONFIG_RAM_PAD)
#endif

#if defined(__ASSEMBLY__)
# define va_to_pa(va) ((va) - CONFIG_PAGE_OFFSET + CONFIG_RAM_BASE)
# define pa_to_va(pa) ((pa) - CONFIG_RAM_BASE + CONFIG_PAGE_OFFSET)
#else
# define va_to_pa(va) ((phys_addr_t)((void *)(va) - CONFIG_PAGE_OFFSET + CONFIG_RAM_BASE))
# define pa_to_va(pa) ((void *)((phys_addr_t)(pa) - CONFIG_RAM_BASE + CONFIG_PAGE_OFFSET))
#endif

#if !defined(CONFIG_ARCH_HAS_IOMAP)
# define io_to_pa(va) va_to_pa(va)
# define pa_to_io(pa) pa_to_va(pa)
#elif defined(__ASSEMBLY__)
# define io_to_pa(va) ((va) - CONFIG_IOREMAP_OFFSET + CONFIG_RAM_BASE)
# define pa_to_io(pa) ((pa) - CONFIG_RAM_BASE + CONFIG_IOREMAP_OFFSET)
#else
# define io_to_pa(va) ((phys_addr_t)((void *)(va) - CONFIG_IOREMAP_OFFSET + CONFIG_RAM_BASE))
# define pa_to_io(pa) ((void *)((phys_addr_t)(pa) - CONFIG_RAM_BASE + CONFIG_IOREMAP_OFFSET))
#endif

#endif /* _ASM_GENERIC_PAGE_H_ */
