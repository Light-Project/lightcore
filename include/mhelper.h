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

#define MARGFN_DECL(type, arg)  type arg
#define MARGFN_LONG(type, arg)  long arg
#define MARGFN_ARGS(type, arg)  arg
#define MARGFN_CAST(type, arg)  (type) arg

#endif  /* _MHELPER_H_ */
