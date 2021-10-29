/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_VMAP_H_
#define _MM_VMAP_H_

#include <kernel.h>
#include <mm/vmem.h>
#include <mm/page.h>

struct vmalloc_area {
    struct vm_area vmem;
    struct page **page;
};

#define vm_to_vmap(vmp) \
    container_of(vmp, struct vmap_area, vmem)


void __malloc *vmalloc_page(struct page *page, int page_nr);
void __malloc *vmalloc(size_t size);
void vfree(void *block);

static inline void *vzalloc

#endif  /* _MM_VMAP_H_ */
