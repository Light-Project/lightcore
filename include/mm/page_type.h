#ifndef _MM_PAGE_TYPE_H_
#define _MM_PAGE_TYPE_H_

#include <types.h>
#include <list.h>

#ifndef __ASSEMBLY__

struct slob_page {
    list_t slob_list;       /* slob page list */
    struct slob_node* node; /* slob node list head of this page */
    int avail;              /* Free space on slob pages */
    uint8_t page_use:1;     /* The whole page is used as a block of node */
};

struct page {
    uint32_t    flags;
    list_t      list;
    unsigned int order;
    union {
        struct slob_page slob;
    };
    uint8_t region_type:3;
};

#endif  /* __ASSEMBLY__ */
#endif  /* _MM_PAGE_DEF_H_ */
