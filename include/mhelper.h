/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MHELPER_H_
#define _MHELPER_H_

#define MMAP_ARG0(fn, ...)
#define MMAP_ARG1(fn, type, arg, ...) fn(type, arg)
#define MMAP_ARG2(fn, type, arg, ...) fn(type, arg), MMAP_ARG1(fn, ##__VA_ARGS__)
#define MMAP_ARG3(fn, type, arg, ...) fn(type, arg), MMAP_ARG2(fn, ##__VA_ARGS__)
#define MMAP_ARG4(fn, type, arg, ...) fn(type, arg), MMAP_ARG3(fn, ##__VA_ARGS__)
#define MMAP_ARG5(fn, type, arg, ...) fn(type, arg), MMAP_ARG4(fn, ##__VA_ARGS__)
#define MMAP_ARG6(fn, type, arg, ...) fn(type, arg), MMAP_ARG5(fn, ##__VA_ARGS__)
#define MMAP_ARGN(n, fn, ...) MMAP_ARG##n(fn, ##__VA_ARGS__)

#define MMAP_DECL0(fn, ...)
#define MMAP_DECL1(fn, type, ...) fn(type, __arg1)
#define MMAP_DECL2(fn, type, ...) fn(type, __arg2), MMAP_DECL1(fn, ##__VA_ARGS__)
#define MMAP_DECL3(fn, type, ...) fn(type, __arg3), MMAP_DECL2(fn, ##__VA_ARGS__)
#define MMAP_DECL4(fn, type, ...) fn(type, __arg4), MMAP_DECL3(fn, ##__VA_ARGS__)
#define MMAP_DECL5(fn, type, ...) fn(type, __arg5), MMAP_DECL4(fn, ##__VA_ARGS__)
#define MMAP_DECL6(fn, type, ...) fn(type, __arg6), MMAP_DECL5(fn, ##__VA_ARGS__)
#define MMAP_DECLN(n, fn, ...) MMAP_DECL##n(fn, ##__VA_ARGS__)

#define MARGFN_DECL(type, arg) type arg
#define MARGFN_LONG(type, arg) long arg
#define MARGFN_ARGS(type, arg) arg
#define MARGFN_CAST(type, arg) (type) arg

#endif /* _MHELPER_H_ */
