/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KSHELL_H_
#define _KSHELL_H_

#include <list.h>
#include <rbtree.h>

struct readline_state;
typedef unsigned int (*kshell_read_t)(char *str, unsigned int len, void *data);
typedef void (*kshell_write_t)(const char *str, unsigned int len, void *data);

struct kshell_context {
    kshell_read_t read;
    kshell_write_t write;
    void *data;

    struct readline_state *readline;
    struct rb_root env;

    unsigned int pipesize;
    unsigned int pipepos;
    char *pipeline;

#ifdef CONFIG_KCMD_FUNC
    struct rb_root func;
#endif
};

struct kshell_command {
    const char *name;
    const char *desc;
    struct list_head list, complete;
    state (*exec)(struct kshell_context *ctx, int argc, char *argv[]);
    state (*prepare)(struct kshell_context *ctx);
    void (*release)(struct kshell_context *ctx);
};

#ifndef CONFIG_KSHELL

static inline void ksh_init(void) {}

#else  /* !CONFIG_KSHELL */

extern char *kshell_getenv(struct kshell_context *ctx, const char *name);
extern state kshell_putenv(struct kshell_context *ctx, char *string);
extern state kshell_setenv(struct kshell_context *ctx, const char *name, const char *val, bool overwrite);
extern state kshell_unsetenv(struct kshell_context *ctx, const char *name);

extern state kshell_exec(struct kshell_context *ctx, const struct kshell_command *cmd, int argc, char *argv[]);
extern state kshell_execv(struct kshell_context *ctx, const char *name, int argc, char *argv[]);
extern state kshell_system(struct kshell_context *ctx, const char *cmdline);
extern bool kshell_ctrlc(struct kshell_context *ctx);

extern int kshell_vprintf(struct kshell_context *ctx, const char *str, va_list args);
extern int __printf(2, 3) kshell_printf(struct kshell_context *ctx, const char *str, ...);
extern state kshell_register(struct kshell_command *cmd);
extern void kshell_unregister(struct kshell_command *cmd);
extern void ksh_init(void);

#endif  /* CONFIG_KSHELL */
#endif  /* _KSHELL_H_ */
