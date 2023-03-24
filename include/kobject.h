/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KOBJECT_H_
#define _KOBJECT_H_

#include <list.h>
#include <spinlock.h>

struct kobject {
    const char *name;
    struct kobject *parent;
    struct list_head list;
};

static inline const char *kobject_get_name(const struct kobject *kobj)
{
    return kobj->name;
}

extern state kobject_set_name_vargs(struct kobject *kobj, const char *fmt, va_list vargs);
extern state kobject_set_name(struct kobject *kobj, const char *fmt, ...);

#endif /* _KOBJECT_H_ */
