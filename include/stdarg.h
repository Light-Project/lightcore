/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STDARG_H_
#define _STDARG_H_

#ifndef __ASSEMBLY__

typedef __builtin_va_list	va_list;

/*
 * prepare to access variable args
 */
#define va_start(v, l)		__builtin_va_start(v, l)

/*
 * the caller will get the value of current argument
 */
#define va_arg(v, l)		__builtin_va_arg(v, l)

/*
 * end for variable args
 */
#define va_end(v)			__builtin_va_end(v)

/*
 * copy variable args
 */
#define va_copy(d, s)		__builtin_va_copy(d, s)

#endif  /* __ASSEMBLY__ */
#endif  /* _STDARG_H_ */
