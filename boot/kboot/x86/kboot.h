/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _KBOOT_H_
#define _KBOOT_H_

#include <lib.h> 

#define bss_start   ((char *)ld_bss_start + (size_t)&head)
#define bss_end     ((char *)ld_bss_end + (size_t)&head)
#define bss_size    (bss_end - bss_start)

#define piggy_start ((char *)ld_piggy_start + (size_t)&head)
#define piggy_end   ((char *)ld_piggy_end + (size_t)&head)
#define piggy_size  (piggy_end - piggy_start)

#define heap_start  ((char *)ld_heap_start + (size_t)&head)
#define heap_end    ((char *)ld_heap_end + (size_t)&head)
#define heap_size   (heap_end - heap_start)

#define load_addr   (size_1MiB * 16)
#define page_offset ((void *)CONFIG_PAGE_OFFSET)
#define kernel_entry (page_offset + 0x1000000)


#ifndef __ASSEMBLY__

/* startup.S */
void head(void);
void kernel_start(void *addr);
void halt(void);

/* kaslr.c */
uint64_t kasir_random(void);

/* page.c */
void kernel_map(void);

/* segment.c */
void segment_init(void);

/* video.c */
void video_print(const char *str);
void video_clear(void);

#endif  /* __ASSEMBLY__ */
#endif  /* _KBOOT_H_ */
