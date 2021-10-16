/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIME_H_
#define _TIME_H_

#include <types.h>

struct timespec {
    time_t  tv_sec;
    long    tv_nsec;
};

time_t mktime(unsigned int year, unsigned int mon, unsigned int day,
              unsigned int hour, unsigned int min, unsigned int sec);

#endif  /* _TIME_H_ */
