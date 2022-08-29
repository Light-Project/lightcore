/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MEMDUMP_H_
#define _MEMDUMP_H_

#include <types.h>

enum memdump_prefix_mode {
    MEMDUMP_PREFIX_NONE,
    MEMDUMP_PREFIX_OFFSET,
    MEMDUMP_PREFIX_ADDRESS,
};

typedef int (*memdump_print_t)(void *pdata, const char *fmt, ...);
extern int memdump_buffer(char *buffer, size_t buffsize, const void *data, size_t data_len, int rowsize, int groupsize, int base, bool ascii);
extern int memdump_print(memdump_print_t func, void *pdata, const char *prefix, enum memdump_prefix_mode mode, const void *data, size_t data_len, int rowsize, int groupsize, int base, bool ascii);
extern int memdump_printk(const char *level, const char *prefix, enum memdump_prefix_mode mode, const void *data, size_t data_len, int rowsize, int groupsize, int base, bool ascii);

#endif	/* _MEMDUMP_H_ */
