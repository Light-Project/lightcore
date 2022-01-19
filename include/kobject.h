/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KERNEL_KOBJECT_H_
#define _KERNEL_KOBJECT_H_

#include <list.h>
#include <spinlock.h>

struct kobject {
    const char *name;
    struct kobject *parent;
    struct list_head list;
};

#endif  /* _KERNEL_KOBJECT_H_ */
