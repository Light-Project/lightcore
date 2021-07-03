#ifndef _ASM_CSKY_PAGE_H_
#define _ASM_CSKY_PAGE_H_

#include <const.h>
#include <asm/cache.h>

/*
 * PAGE_SHIFT determines the page size: 4KB
 */
#define PAGE_SHIFT      12
#define PAGE_SIZE       (1 << PAGE_SHIFT)
#define PAGE_MASK       (~(PAGE_SIZE - 1))
#define THREAD_SIZE     (PAGE_SIZE * 2)
#define THREAD_MASK     (~(THREAD_SIZE - 1))
#define THREAD_SHIFT    (PAGE_SHIFT + 1)


/*
 * For C-SKY "User-space:Kernel-space" is "2GB:2GB" fixed by hardware and there
 * are two segment registers (MSA0 + MSA1) to mapping 512MB + 512MB physical
 * address region. We use them mapping kernel 1GB direct-map address area and
 * for more than 1GB of memory we use highmem.
 */
#define PAGE_OFFSET     CONFIG_PAGE_OFFSET
#define SSEG_SIZE       0x20000000
#define LOWMEM_LIMIT    (SSEG_SIZE * 2)

#define PHYS_OFFSET_OFFSET (CONFIG_RAM_BASE & (SSEG_SIZE - 1)) 


#endif
