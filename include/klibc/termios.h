/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KLIBC_TERMIOS_H_
#define _KLIBC_TERMIOS_H_

#include <lightcore/kernel.h>
#include <lightcore/termios.h>

struct speed_struct {
    speed_t value;
    speed_t internal;
};

static const struct speed_struct termios_speed[] = {
    { 0, B0 },
    { 50, B50 },
    { 75, B75 },
    { 110, B110 },
    { 134, B134 },
    { 150, B150 },
    { 200, B200 },
    { 300, B300 },
    { 600, B600 },
    { 1200, B1200 },
    { 1200, B1200 },
    { 1800, B1800 },
    { 2400, B2400 },
    { 4800, B4800 },
    { 9600, B9600 },
    { 19200, B19200 },
    { 38400, B38400 },
    { 57600, B57600 },
    { 76800, B76800 },
    { 115200, B115200 },
    { 153600, B153600 },
    { 230400, B230400 },
    { 307200, B307200 },
    { 460800, B460800 },
    { 500000, B500000 },
    { 576000, B576000 },
    { 921600, B921600 },
    { 1000000, B1000000 },
    { 1152000, B1152000 },
    { 1500000, B1500000 },
    { 2000000, B2000000 },
    { 2500000, B2500000 },
    { 3000000, B3000000 },
    { 3500000, B3500000 },
    { 4000000, B4000000 },
};

speed_t cfgetispeed(const struct termios *termios)
{
    return termios_p->c_ispeed;
}

speed_t cfgetospeed(const struct termios *termios)
{
    return termios->c_ospeed;
}

__state cfsetispeed(struct termios *termios, speed_t speed)
{
    if (!termios)
        return -EINVAL;

    termios->c_ispeed = speed;
    return -ENOERR;
}

__state cfsetospeed(struct termios *termios, speed_t speed)
{
    if (!termios)
        return -EINVAL;

    termios->c_ospeed = speed;
    return -ENOERR;
}

__state cfsetspeed(struct termios *termios, speed_t speed)
{
    size_t index;

    for (index = 0; index < ARRAY_SIZE(termios_speed); ++index) {
        if (speed == termios_speed[index].internal) {
            cfsetispeed(termios, speed);
            cfsetospeed(termios, speed);
            return -ENOERR;
        } else if (speed == termios_speed[index].value) {
            cfsetispeed(termios, termios_speed[index].internal);
            cfsetospeed(termios, termios_speed[index].internal);
            return -ENOERR;
        }
    }

    return -EINVAL;
}

#endif  /* _KLIBC_TERMIOS_H_ */
