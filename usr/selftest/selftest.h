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
    state (*testing)(struct kshell_context *ctx, void *pdata);
    void *(*prepare)(struct kshell_context *ctx, int argc, char *argv[]);
    void (*release)(struct kshell_context *ctx, void *pdata);
};

extern state selftest_register(struct selftest_command *command);
extern void selftest_unregister(struct selftest_command *command);

#endif  /* _LOCAL_BENCHMARK_H_ */
