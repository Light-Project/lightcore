/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <time.h>

unsigned int tick_to_msecs()
{

}

time_t mktime(unsigned int year, unsigned int mon, unsigned int day,
              unsigned int hour, unsigned int min, unsigned int sec)
{
    time_t time;

    if (0 >= (int)(mon -= 2)) {
        mon += 12;
        year -= 1;
    }

    time = (year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day) + year * 365 - 719499;

    time = (time * 24) + hour;  /* now it's hours */
    time = (time * 60) + min;   /* now it's minutes */
    time = (time * 60) + sec;   /* now it's seconds */

    return time;
}


