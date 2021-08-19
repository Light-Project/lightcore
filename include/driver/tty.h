/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_TTY_H_
#define _DRIVER_TTY_H_

struct tty_device;

struct tty_ops {
    void (*putc)(struct tty_device *tty, int xpos, int ypos);
};

struct tty_device {

};

#endif  /* _DRIVER_TTY_H_ */
