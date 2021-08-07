/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIME_H_
#define _TIME_H_

#include <types.h>

typedef uint64_t time_t;

struct timespec {
    time_t  tv_sec;
    long    tv_nsec;
};

#endif  /* _TIME_H_ */
