/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_VMEM_H_
#define _MM_VMEM_H_

#include <state.h>
#include <list.h>
#include <rbtree.h>
#include <mm/gfp.h>

struct vm_area {
    size_t addr, size;
    struct rb_node rb;
    struct list_head list;
};

#define rb_to_vma(node) \
    rb_entry_safe(node, struct vm_area, rb)

extern struct vm_area *vmem_area_find(size_t addr);
extern state vmem_area_alloc(struct vm_area *va, size_t size, size_t align);
extern void vmem_area_free(struct vm_area *va);
extern struct vm_area *vmem_alloc_node(size_t size, size_t align, gfp_t gfp);
extern struct vm_area *vmem_alloc(size_t size);
extern void vmem_free(struct vm_area *va);

extern void __init vmem_init(void);

#endif  /* _MM_VMEM_H_ */