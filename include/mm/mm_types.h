/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_TYPES_H_
#define _MM_TYPES_H_

#include <types.h>
#include <list.h>

struct slob_page {
    list_t slob_list;       /* slob page list */
    struct slob_node* node; /* slob node list head of this page */
    int avail;              /* Free space on slob pages */
    uint8_t page_use:1;     /* The whole page is used as a block of node */
};

struct page {
    uint32_t order:4;
    uint32_t region_type:3;

    struct list_head free_list;

    union {
        struct slob_page slob;
    };
};

struct page_free {
    struct list_head list;  /* free page list */
    size_t free_nr;         /* free page number */
};

#endif  /* _MM_PAGE_DEF_H_ */
