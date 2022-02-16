/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KBOOT_H_
#define _KBOOT_H_

#include <kboot16.h>
#include <lib.h>

extern char _ld_piggy_start;
extern char _ld_piggy_end;
extern char _ld_heap_start;
extern char _ld_heap_end;

#define piggy_start (&_ld_piggy_start)
#define piggy_end   (&_ld_piggy_end)
#define piggy_size  (piggy_end - piggy_start)

#define heap_start  (&_ld_heap_start)
#define heap_end    (&_ld_heap_end)
#define heap_size   (heap_end - heap_start)

extern struct e820_table e820_table;
extern void __noreturn kernel_start(uint16_t segment, uint32_t offset);
extern void __noreturn halt(void);
extern void kernel_map(void);
extern void segment_init(void);
extern void console_print(const char *str);
extern void console_clear(void);

#endif  /* _KBOOT_H_ */
