/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _COMPILER_INCBIN_H_
#define _COMPILER_INCBIN_H_

#define INCBIN_SECTION(section, name, align, file)  \
    asm volatile (                                  \
        ".section   "__stringify(section)"      \n" \
        ".balign    "__stringify(align)"        \n" \
        ".global    "__stringify(name)"         \n" \
        __stringify(name)"_start:               \n" \
        ".incbin    \""file"\"                  \n" \
        __stringify(name)"_end:                 \n" \
    );                                              \
    extern void *name##_start[];                    \
    extern void *name##_end[]

#endif  /* _COMPILER_INCBIN_H_ */
