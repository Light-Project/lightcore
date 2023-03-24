/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IHEX_PARSER_H_
#define _IHEX_PARSER_H_

#include <types.h>
#include <stddef.h>

enum ihex_type {
    IHEX_TYPE_DATA      = 0, /* Data */
    IHEX_TYPE_EOF       = 1, /* End Of File */
    IHEX_TYPE_ESEG      = 2, /* Extended Segment Address */
    IHEX_TYPE_SSEG      = 3, /* Start Segment Address */
    IHEX_TYPE_EADDR     = 4, /* Extended Linear Address */
    IHEX_TYPE_SADDR     = 5, /* Start Linear Address */
};

struct ihex_head {
    char code;
    char length[2];
    char offset[4];
    char type[2];
    char data[0];
} __packed;

struct ihex_state {
    uint16_t offset;
    uint16_t segm_exten;
    uint16_t segm_start;
    uint16_t addr_exten;
    uint32_t addr_start;
};

typedef state (*ihex_report_t)(struct ihex_state *ihex, enum ihex_type type, const void *data, size_t length, void *pdata);
extern state ihex_parser(const void *image, ihex_report_t report, void *pdata);

#endif /* _IHEX_PARSER_H_ */
