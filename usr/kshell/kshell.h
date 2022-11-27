/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LOCAL_KSHELL_H_
#define _LOCAL_KSHELL_H_

#include <kshell.h>
#include <spinlock.h>
#include <console.h>

enum readline_esc {
    READLINE_ESC_NORM   = 0,
    READLINE_ESC_ESC    = 1,
    READLINE_ESC_CSI    = 2,
    READLINE_ESC_SS3    = 3,
};

struct readline_history {
    struct list_head list;
    unsigned int len;
    char cmd[];
};

struct readline_state {
    kshell_read_t read;
    kshell_write_t write;
    void *data;

    const char *prompt;
    unsigned int plen;

    char *buff;
    unsigned int len;
    unsigned int pos;
    unsigned int bsize;
    bool keylock;
    char esc_param;
    enum readline_esc esc_state;

    const char *workspace;
    unsigned int worklen;
    unsigned int worksize;

    const char *clipbrd;
    unsigned int cliplen;
    unsigned int clippos;
    unsigned int clipsize;
    bool clipview;

    struct list_head history;
    struct readline_history *curr;
};

struct kshell_stack {
    struct list_head list;
    struct rb_root env;
};

struct kshell_env {
    struct rb_node node;
    char *val, name[0];
};

struct kshell_func {
    struct rb_node node;
    char *body, name[0];
};

#define env_to_kshell(ptr) \
    rb_entry(ptr, struct kshell_env, node)

#define func_to_kshell(ptr) \
    rb_entry(ptr, struct kshell_func, node)

#define list_to_kshell(ptr) \
    list_entry(ptr, struct kshell_command, list)

extern spinlock_t kshell_lock;
extern struct list_head kshell_list;

extern unsigned int readline_read(struct readline_state *rstate, char *str, unsigned int len);
extern void readline_write(struct readline_state *rstate, const char *str, unsigned int len);
extern char *readline(struct readline_state *state, const char *dprompt, const char *cprompt);
extern struct readline_state *readline_alloc(kshell_read_t read, kshell_write_t write, void *data);
extern void readline_free(struct readline_state *state);

extern state kshell_parser(struct kshell_context *ctx, const char **pcmdline, int *argc, char ***argv);
extern state kshell_main(struct kshell_context *ctx, int argc, char *argv[]);
extern struct kshell_command *kshell_find(const char *name);

#endif  /* _KSHELL_PRIV_H_ */
