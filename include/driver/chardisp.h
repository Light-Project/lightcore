/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CHARDISP_H_
#define _CHARDISP_H_

#include <device.h>

enum chardisp_font {
    CHARDISP_FONT_MIDDLE        = 0,
    CHARDISP_FONT_SMALL         = 1,
    CHARDISP_FONT_BIG           = 2,
};

struct chardisp_device {
	const struct chardisp_ops *ops;

    unsigned int height;
    unsigned int width;
    unsigned int xpos;
    unsigned int ypos;
};

struct chardisp_ops {
    state (*backlight)(struct chardisp_device *lcd, bool onoff);
    state (*display)(struct chardisp_device *lcd, bool onoff);
    state (*cursor)(struct chardisp_device *lcd, bool onoff);
    state (*blink)(struct chardisp_device *lcd, bool onoff);

    state (*print)(struct chardisp_device *lcd, int ch);
    state (*home)(struct chardisp_device *lcd);
    state (*gotoxy)(struct chardisp_device *lcd, unsigned int xpos, unsigned int ypos);
    state (*fontsize)(struct chardisp_device *lcd, enum chardisp_font size);
};

extern state chardisp_register(struct chardisp_device *cdev);
extern void chardisp_unregister(struct chardisp_device *cdev);

#endif /* _CHARDISP_H_ */
