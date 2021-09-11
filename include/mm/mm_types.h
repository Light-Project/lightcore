/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_TYPES_H_
#define _MM_TYPES_H_

#include <types.h>
#include <list.h>

struct slob_page {
    list_t slob_list;       /* slob page list */
    struct slob_node* node; /* slob node list head of this page */
    int avail;              /* Free space on slob pages */
};

struct page {
    uint32_t region_type:3;
    uint32_t order:4;
    uint32_t free:1;

#ifdef CONFIG_SPARCEMEM
    uint32_t sparce_nr:8;
#endif

    union {
        struct {
            struct list_head free_list;
        };
        struct slob_page slob;
    };
};

struct page_free {
    struct list_head list;  /* free page list */
    size_t free_nr;         /* free page number */
};

struct uvm_area {
    size_t addr;    /* virtual start address */
    size_t size;    /* virtual allocation size */
    struct list_head list;
};

#endif  /* _MM_PAGE_DEF_H_ */
