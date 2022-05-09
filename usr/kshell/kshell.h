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

    char *buff;
    const char *workspace;
    unsigned int worklen;
    unsigned int worksize;
    unsigned int bsize;
    const char *prompt;
    unsigned int plen;
    unsigned int len;
    unsigned int pos;
    char esc_param;
    enum readline_esc esc_state;
    struct list_head history;
    struct readline_history *curr;
};

struct kshell_env {
    struct rb_node node;
    char *val, name[];
};

#define env_to_kshell(ptr) \
    rb_entry(ptr, struct kshell_env, node)

#define list_to_kshell(ptr) \
    list_entry(ptr, struct kshell_command, list)

extern struct list_head kshell_list;
extern struct spinlock kshell_lock;

extern unsigned int readline_read(struct readline_state *rstate, char *str, unsigned int len);
extern void readline_write(struct readline_state *rstate, const char *str, unsigned int len);
extern char *readline(struct readline_state *state, const char *prompt);
extern struct readline_state *readline_alloc(kshell_read_t read, kshell_write_t write, void *data);
extern void readline_free(struct readline_state *state);

extern state kshell_envclone(struct kshell_context *ctx, struct kshell_context *new);
extern void kshell_envrelease(struct kshell_context *ctx);
extern state kshell_parser(struct kshell_context *ctx, const char *cmdline, const char **pos, int *argc, char ***argv);
extern state kshell_main(struct kshell_context *ctx, int argc, char *argv[]);
extern struct kshell_command *kshell_find(const char *name);

#endif  /* _KSHELL_PRIV_H_ */
