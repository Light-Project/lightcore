/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LOCAL_MANUAL_H_
#define _LOCAL_MANUAL_H_

#include <list.h>
#include <initcall.h>

struct kshell_manual {
    struct list_head list;
    const char *name;
    const char *describe;
    unsigned int length;
};

#define DEFINE_MANUAL(mname, ...)                   \
static struct kshell_manual mname##_manual = {      \
    .name = __stringify(mname),                     \
    .describe = __VA_ARGS__,                        \
    .length = sizeof(__VA_ARGS__) - 1,              \
};                                                  \
static state mname##_manual_init(void)              \
{                                                   \
    return kshell_manual_regsiter(&mname##_manual); \
}                                                   \
kshell_initcall(mname##_manual_init)

extern state kshell_manual_regsiter(struct kshell_manual *manual);
extern void kshell_manual_unregsiter(struct kshell_manual *manual);

#endif  /* _LOCAL_MANUAL_H_ */
