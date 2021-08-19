/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LOGO_H_
#define _LOGO_H_

#include <types.h>

struct logo {
    const char *name;
    uint32_t width;
    uint32_t height;
    uint8_t colour;
    void *data;
};

extern const struct logo logo_info;

void terminal_logo(void);

#endif  /* _LOGO_H_ */
