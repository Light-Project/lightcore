/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KTHREAD_H_
#define _KTHREAD_H_

#include <state.h>

enum kthread_flags {
};

struct kthread {
    enum kthread_flags flasg;
    struct
};

__printf(2, 3) struct kthread *
kthread_create(const char *name, ...);

#endif  /* _KTHREAD_H_ */
