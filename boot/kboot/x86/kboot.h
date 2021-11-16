/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KBOOT_H_
#define _KBOOT_H_

#include <kboot16.h>
#include <lib.h>


#define kboot_base  (((size_t)&_start - 0x200))
#define bss_start   (ld_bss_start   + kboot_base)
#define bss_end     (ld_bss_end     + kboot_base)
#define piggy_start (ld_piggy_start + kboot_base)
#define piggy_end   (ld_piggy_end   + kboot_base)
#define heap_start  (ld_heap_start  + kboot_base)
#define heap_end    (ld_heap_end    + kboot_base)

#define bss_size    (bss_end - bss_start)
#define piggy_size  (piggy_end - piggy_start)
#define heap_size   (heap_end - heap_start)

extern struct e820_table e820_table;

/* startup.S */
extern void _start(void);
extern void __noreturn kernel_start(void);
extern void __noreturn halt(void);

/* kaslr.c */
extern uint64_t kasir_random(void);

/* page.c */
extern void kernel_map(void);

/* segment.c */
extern void segment_init(void);

/* video.c */
extern void console_print(const char *str);
extern void console_clear(void);

#endif  /* _KBOOT_H_ */
