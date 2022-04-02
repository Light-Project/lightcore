/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KSHELL_PRIV_H_
#define _KSHELL_PRIV_H_

#include <kshell.h>
#include <spinlock.h>
#include <console.h>

typedef unsigned int (*readline_read_t)(char *str, unsigned int len, void *data);
typedef void (*readline_write_t)(const char *str, unsigned int len, void *data);

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
    readline_read_t read;
    readline_write_t write;
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
    struct list_head list;
    char *val, name[];
};

extern struct list_head kshell_list;
extern struct list_head kshell_env_list;
extern struct spinlock kshell_lock;
extern struct spinlock kshell_env_lock;

extern char *readline(struct readline_state *state, const char *prompt);
extern struct readline_state *readline_alloc(readline_read_t read, readline_write_t write, void *data);
extern void readline_free(struct readline_state *state);

extern state kshell_parser(const char *cmdline, const char **pos, int *argc, char ***argv);
extern state kshell_main(int argc, char *argv[]);
extern struct kshell_command *kshell_find(const char *name);

#endif  /* _KSHELL_PRIV_H_ */
