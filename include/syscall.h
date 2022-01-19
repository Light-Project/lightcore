/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <types.h>
#include <state.h>
#include <initcall.h>
#include <export.h>
#include <lightcore/syscall.h>

struct syscall_entry {
    const char *name;
    unsigned int args;
    const void *entry;
};

extern long syscall_fork(void);
extern long syscall_vfork(void);

extern long syscall_undefine_handle(void);
extern state syscall_register(unsigned int callnr, const char *name, unsigned int args, void *entry);
extern void syscall_unregister(unsigned int callnr);

#define SYSCALL_ENTRY(callnr, entry, args)                  \
static state init_##entry(void)                             \
{                                                           \
    return syscall_register(callnr, #entry, args, entry);   \
}                                                           \
EXPORT_SYMBOL(entry);                                       \
core_initcall(init_##entry)

#endif  /* _SYSCALL_H_ */
