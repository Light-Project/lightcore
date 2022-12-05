/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FMTDUMP_H_
#define _FMTDUMP_H_

#include <types.h>
#include <stddef.h>

typedef int (*fmtdump_print_t)(void *pdata, const char *fmt, ...);

enum fmtdump_prefix_mode {
    FMTDUMP_PREFIX_NONE,
    FMTDUMP_PREFIX_OFFSET,
    FMTDUMP_PREFIX_ADDRESS,
};

struct fmtdump_ops {
    uint8_t (*read8)(void *pdata, uintptr_t addr);
    uint16_t (*read16)(void *pdata, uintptr_t addr);
    uint32_t (*read32)(void *pdata, uintptr_t addr);
    uint64_t (*read64)(void *pdata, uintptr_t addr);
    fmtdump_print_t print;
};

extern int fmtdump_line(const struct fmtdump_ops *ops, void *pdata, char *buffer, size_t buffsize,
                        uintptr_t addr, size_t size, int rowsize, int groupsize, int base, bool ascii);
extern int fmtdump_print(const struct fmtdump_ops *ops, void *pdata, const char *prefix, enum fmtdump_prefix_mode mode,
                         uintptr_t addr, size_t size, int rowsize, int groupsize, int base, bool ascii);

#endif	/* _FMTDUMP_H_ */
