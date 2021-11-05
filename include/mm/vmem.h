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
};

#define vm_rb_entry(node) \
    rb_entry(node, struct vm_area, rb_node)

extern struct vm_area *vmem_area_find(size_t addr);
extern state vmem_area_alloc(struct vm_area *va, size_t size, size_t align);
extern void vmem_area_free(struct vm_area *va);
extern struct vm_area *vmem_alloc_node(size_t size, size_t align, gfp_t gfp);
extern struct vm_area *vmem_alloc(size_t size);
extern void vmem_free(struct vm_area *va);

extern void __init vmem_init(void);

#endif  /* _MM_VMEM_H_ */