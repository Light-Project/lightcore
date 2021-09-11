/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_SLOB_H_
#define _MM_SLOB_H_

#include <list.h>
#include <asm/page.h>
#include <mm/gfp.h>

#if ((PAGE_SHIFT < 15) && CONFIG_ARCH_X86)
#define slobidx_t   uint16_t
#else
#define slobidx_t   uint32_t
#endif

/* A slob unit head */
struct slob_node {
    slobidx_t size:PAGE_SHIFT;  /* node offset of page */
    slobidx_t use:1;
};

void *slob_alloc(size_t size, gfp_t gfp, int align);
void slob_free(void *block);

#endif /* _MM_SLOB_H_ */
