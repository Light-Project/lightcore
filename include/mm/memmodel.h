#ifndef _MM_MEMMODEL_H_
#define _MM_MEMMODEL_H_

#include <mm/page_type.h>

#ifndef __ASSEMBLY__

/* Flat Memory model */
#if defined(CONFIG_FLATMEM)

#define page_to_nr(_page)   ((unsigned int)((_page - page_map) / sizeof(struct page)))
#define nr_to_page(pnr)     (page_map[pnr])

/* Sparce Memory model */
#elif defined(CONFIG_SPARCEMEM)


#endif /* CONFIG_FLATMEM/CONFIG_SPARCEMEM */

#define page_to_pa(page)    ((phys_addr_t)page_to_nr(page) << PAGE_SHIFT)
#define pa_to_page(pa)      (nr_to_page(pa >> PAGE_SHIFT))

#define page_to_va(page)    (pa_to_va(page_to_pa(page)))
#define va_to_page(address) (pa_to_page(va_to_pa(address)))

#endif  /* __ASSEMBLY__ */
#endif  /* _MM_MEMMODEL_H_ */
