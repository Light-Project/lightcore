/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <rbtree.h>
#include <initcall.h>
#include <export.h>
#include <mhelper.h>
#include <lightcore/syscall.h>

struct syscall_desc {
    struct rb_node node;
    unsigned long callnr;
    unsigned int argnr;
    const char *name;
    const void *entry;
    atomic_t count;
};

#define node_to_rbtree(ptr) \
    container_of(ptr, struct syscall_desc, node)

#define DEFINE_SYSCALL_INIT(NAME, ARGNR)        \
struct syscall_desc __##NAME##_desc = {         \
    .callnr = syscall_##NAME,                   \
    .argnr  = (ARGNR),                          \
    .name   = __stringify(NAME),                \
    .entry  = __##NAME##_entry,                 \
};                                              \
static state __##NAME##_init(void)              \
{                                               \
    return syscall_register(&__##NAME##_desc);  \
}                                               \
core_initcall(__##NAME##_init);

#define DEFINE_SYSCALLN(NAME, ARGNR, ...)                                       \
static long __##NAME##_entry(MMAP_ARGN(ARGNR, MARGFN_LONG, ##__VA_ARGS__));     \
static long __##NAME##_handle(MMAP_ARGN(ARGNR, MARGFN_DECL, ##__VA_ARGS__));    \
DEFINE_SYSCALL_INIT(NAME, ARGNR);                                               \
static long __##NAME##_entry(MMAP_ARGN(ARGNR, MARGFN_LONG, ##__VA_ARGS__))      \
{                                                                               \
    return __##NAME##_handle(MMAP_ARGN(ARGNR, MARGFN_CAST, ##__VA_ARGS__));     \
}                                                                               \
static long __##NAME##_handle(MMAP_ARGN(ARGNR, MARGFN_DECL, ##__VA_ARGS__))

#define DEFINE_SYSCALL0(NAME, ...) \
    DEFINE_SYSCALLN(NAME, 0, ##__VA_ARGS__)
#define DEFINE_SYSCALL1(NAME, ...) \
    DEFINE_SYSCALLN(NAME, 1, ##__VA_ARGS__)
#define DEFINE_SYSCALL2(NAME, ...) \
    DEFINE_SYSCALLN(NAME, 2, ##__VA_ARGS__)
#define DEFINE_SYSCALL3(NAME, ...) \
    DEFINE_SYSCALLN(NAME, 3, ##__VA_ARGS__)
#define DEFINE_SYSCALL4(NAME, ...) \
    DEFINE_SYSCALLN(NAME, 4, ##__VA_ARGS__)
#define DEFINE_SYSCALL5(NAME, ...) \
    DEFINE_SYSCALLN(NAME, 5, ##__VA_ARGS__)
#define DEFINE_SYSCALL6(NAME, ...) \
    DEFINE_SYSCALLN(NAME, 6, ##__VA_ARGS__)

extern long syscall_handle(unsigned long callnr, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6);
extern state syscall_register(struct syscall_desc *syscall);
extern void syscall_unregister(struct syscall_desc *syscall);

#endif /* _SYSCALL_H_ */
