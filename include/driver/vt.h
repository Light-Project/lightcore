/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_VT_H_
#define _DRIVER_VT_H_

#include <device.h>

#define VT_PAR_NR 16

enum vt_state {
    VT_STATE_NORMAL,
    VT_STATE_ESC,
    VT_STATE_SQUARE,
    VT_STATE_GETPARS,
    VT_STATE_FUNCKEY,
	VT_STATE_HASH,
    VT_STATE_SETG0,
    VT_STATE_SETG1,
    VT_STATE_PERCENT,
    VT_STATE_CSIIGNORE,
    VT_STATE_NONSTD,
	VT_STATE_PALETTE,
    VT_STATE_OSC,
    VT_STATE_APC,
    VT_STATE_PM,
    VT_STATE_DCS,
};

struct vt_cursor {
    unsigned xpos, ypos;
};

/**
 * struct vt_context -
 */
struct vt_context {
    unsigned int cols, rows;
    unsigned int top, bottom;

    struct vt_cursor cur;
    struct vt_cursor cur_save;

    enum vt_state state;
    unsigned int par_nr;
    unsigned int par[VT_PAR_NR];
};

/**
 * struct vt_device -
 */
struct vt_device {
    struct vt_ops *ops;
    struct vt_context context;
};

/**
 * struct vt_ops - virtual terminal operations
 */
struct vt_ops {

    void (*clear)(struct vt_device *vt, unsigned int xpos, unsigned int ypos, unsigned int width, unsigned int height);
    void (*putc)(struct vt_device *vt, char ch, unsigned int xpos, unsigned int ypos);
    void (*putcs)(struct vt_device *vt, const char *str, unsigned int len, unsigned int xpos, unsigned int ypos);
};

#endif /* _DRIVER_VT_H_ */
