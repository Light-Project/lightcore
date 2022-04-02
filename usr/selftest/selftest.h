/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LOCAL_SELFTEST_H_
#define _LOCAL_SELFTEST_H_

#include <error.h>
#include <kshell.h>

struct selftest_command {
    struct list_head list;
    const char *group;
    const char *name;
    const char *desc;
    state (*testing)(void *pdata);
    void *(*prepare)(int argc, char *argv[]);
    void (*release)(void *pdata);
};

extern state selftest_register(struct selftest_command *command);
extern void selftest_unregister(struct selftest_command *command);

#endif  /* _LOCAL_BENCHMARK_H_ */
