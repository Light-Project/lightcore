/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KBOOT_H_
#define _KBOOT_H_

#include <lib.h>

#define bss_start   ((char *)ld_bss_start)
#define bss_end     ((char *)ld_bss_end)
#define bss_size    (bss_end - bss_start)

#define piggy_start ((char *)ld_piggy_start)
#define piggy_end   ((char *)ld_piggy_end)
#define piggy_size  (piggy_end - piggy_start)

#define heap_start  ((char *)ld_heap_start)
#define heap_end    ((char *)ld_heap_end)
#define heap_size   (heap_end - heap_start)

/* startup.S */
void head(void);
void kernel_start(void *addr);
void halt(void);

/* page.c */
void kernel_map(void);

#endif /* _KBOOT_H_ */
