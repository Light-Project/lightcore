/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_VMEM_H_
#define _MM_VMEM_H_

#include <state.h>
#include <list.h>
#include <rbtree.h>

#include <mm/gfp.h>

enum vm_flags {
    VM_MALLOC   = 0x01,
    VM_IOMAP    = 0x02,
};

struct vm_area {
    size_t addr;    /* virtual start address */
    size_t size;    /* virtual allocation size */
    struct rb_node rb_node; /* address sorted rbtree */
    struct list_head list;  /* address sorted list */

    struct page **page;
    unsigned int page_nr;
};

#define vm_rb_entry(node) \
        rb_entry(node, struct vm_area, rb_node)

/* functions provided for external use */
struct vm_area *vmem_alloc_node(size_t size, size_t align, gfp_t gfp);
struct vm_area *vmem_alloc(size_t size);

state vmem_free(struct vm_area *va);
struct vm_area *vmem_area_find(void *addr);

void vmem_init(void);

#endif  /* _MM_VMEM_H_ */