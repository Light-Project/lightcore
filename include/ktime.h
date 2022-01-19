/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KTIME_H_
#define _KTIME_H_

#include <types.h>
#include <time.h>

typedef uint64_t ktime_t;

static inline ktime_t ktime_make(const uint64_t secs, const unsigned long nsecs)
{
    return secs * NSEC_PER_SEC + nsecs;
}

extern ktime_t ktime_get(void);

#endif  /* _KTIME_H_ */
