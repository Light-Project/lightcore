/*
 *  lightcore/include/stdlib.h
 *  c library
 *  (C) 2020 wzl
 */

#ifndef _MM_H_
#define _MM_H_

#include <types.h>

#include <mm/region.h>
#include <mm/page.h>
#include <mm/kmem.h>

#ifndef __ASSEMBLY__

void *ioremap(phys_addr_t pa, size_t size);
void iounmap(void *addr);


#endif	/* __ASSEMBLY__ */
#endif /* _MM_H_ */
