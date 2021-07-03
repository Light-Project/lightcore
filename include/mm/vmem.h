/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _MM_VMEM_H_
#define _MM_VMEM_H_

#include <list.h>
#include <rbtree.h>
#include <mm/gfp.h>
#include <mm/page_type.h>

#ifndef __ASSEMBLY__

struct mmu {

};


struct vm_area {
    void *addr;
    size_t size;

    struct page **page;
    unsigned int page_nr;
    phys_addr_t phys_addr;

    struct rb_node rb_node;
};

struct vm_area *vmem_alloc(size_t size, gfp_t gfp);
void vmem_free(struct vm_area *va);

#endif  /* __ASSEMBLY__ */
#endif