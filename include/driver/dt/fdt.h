/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_DT_FDT_H_
#define _DRIVER_DT_FDT_H_

#include <types.h>
#include <state.h>

#include <lightcore/asm/byteorder.h>

extern void *dt_start_addr;
extern uint32_t dt_crc32;

static inline uint64_t dt_read(const be32 *cell, int size)
{
    uint64_t ret;
    while (size--)
        ret = (ret << 32) | be32_to_cpu(*cell);
    return ret;
}

/* fdt.c */
state dt_scan_node(state (*fn)(unsigned long node, const char *uname, int depth, void *data), void *data);
const void *dt_get_prop(unsigned long node, const char *name, int *size);
void dt_init(void);

/* early.c */
state early_dt_scan(void *addr);

#endif  /* _DRIVER_DT_LIBFDT_H_ */
