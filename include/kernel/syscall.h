/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KERNEL_SYSCALL_H_
#define _KERNEL_SYSCALL_H_

#include <types.h>
#include <linkage.h>
#include <kernel/syscall_tbl.h>

struct syscall_entry {
};

/*
* The asmlinkage stub is aliased to a function named __se_sys_*() which
* sign-extends 32-bit ints to longs whenever needed. The actual work is
* done within __do_sys_*().
*/
#ifndef __SYSCALL_ENTRYx
#define __SYSCALL_ENTRYx(x, name, ...)					\
    __diag_push();							\
    __diag_ignore(GCC, 8, "-Wattribute-alias",			\
            "Type aliasing is used to sanitize syscall arguments");\
    asmlinkage long sys##name(__MAP(x,__SC_DECL,__VA_ARGS__))	\
        __attribute__((alias(__stringify(__se_sys##name))));	\
    ALLOW_ERROR_INJECTION(sys##name, ERRNO);			\
    static inline long __do_sys##name(__MAP(x,__SC_DECL,__VA_ARGS__));\
    asmlinkage long __se_sys##name(__MAP(x,__SC_LONG,__VA_ARGS__));	\
    asmlinkage long __se_sys##name(__MAP(x,__SC_LONG,__VA_ARGS__))	\
    {								\
        long ret = __do_sys##name(__MAP(x,__SC_CAST,__VA_ARGS__));\
        __MAP(x,__SC_TEST,__VA_ARGS__);				\
        __PROTECT(x, ret,__MAP(x,__SC_ARGS,__VA_ARGS__));	\
        return ret;						\
    }								\
    __diag_pop();							\
    static inline long __do_sys##name(__MAP(x,__SC_DECL,__VA_ARGS__))
#endif /* __SYSCALL_DEFINEx */
    
#define SYSCALL_ENTRY(sysnr, fn, argn) 


/* File basic operation */
asmlinkage long sys_open(const char *filename, int flags, umode_t mode);
asmlinkage long sys_close(unsigned int fd);
asmlinkage long sys_read(unsigned int fd, char *buf, size_t count);
asmlinkage long sys_write(unsigned int fd, const char *buf, size_t count);


#endif
