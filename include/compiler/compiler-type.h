/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _COMPILER_TYPE_H_
#define _COMPILER_TYPE_H_

#ifdef __OPTIMIZE__
# define _compiletime_assert(condition, msg, prefix, suffix)            \
    do {                                                                \
        extern void prefix ## suffix(void) __compiletime_error(msg);    \
        if (!(condition))                                               \
            prefix ## suffix();                                         \
    } while (0)
#else
# define _compiletime_assert(condition, msg, prefix, suffix) while(0);
#endif

#define compiletime_assert(condition, msg) \
	_compiletime_assert(condition, msg, __compiletime_assert_, __COUNTER__)

/* Is this type a native word size -- useful for atomic operations */
#define __native_word(t) \
    (sizeof(t) == sizeof(char) || sizeof(t) == sizeof(short) || \
     sizeof(t) == sizeof(int)  || sizeof(t) == sizeof(long))

/* Indirect macros required for expanded argument pasting, eg. __LINE__. */
#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)

#endif
