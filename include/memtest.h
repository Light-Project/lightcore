/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MEMTEST_H_
#define _MEMTEST_H_

#include <types.h>
#include <stddef.h>

typedef void (*memtest_report_t)(void *start, size_t size);

#ifdef CONFIG_MEMTEST
extern void memtest(memtest_report_t report, unsigned int level,
                    void *block, size_t size, size_t step);
extern void memtest_once(memtest_report_t func, size_t pattern,
                         void *block, size_t size, size_t step);
#else
static inline void
memtest(memtest_report_t report, unsigned int level,
        void *block, size_t size, size_t step) {}
static inline void
memtest_once(memtest_report_t func, size_t pattern,
             void *block, size_t size, size_t step) {}
#endif

#endif /* _MEMTEST_H_ */
