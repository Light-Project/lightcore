/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KSHELL_H_
#define _KSHELL_H_

#include <types.h>
#include <list.h>

struct kshell_command {
    const char *name;
    const char *help;
    struct list_head list;
    int (*entry)(int argc, char *argv[]);
};

state kshell_register(struct kshell_command *);
void kshell_unregister(struct kshell_command *);

#endif  /* _KSHELL_H_ */
