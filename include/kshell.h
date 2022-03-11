/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KSHELL_H_
#define _KSHELL_H_

#include <list.h>

struct kshell_command {
    const char *name;
    const char *desc;
    struct list_head list;
    state (*exec)(int argc, char *argv[]);
};

#define list_to_kshell(ptr) \
    container_of(ptr, struct kshell_command, list)

#ifndef CONFIG_KSHELL

static inline void ksh_init(void) {}

#else  /* !CONFIG_KSHELL */

extern char *kshell_getenv(const char *name);
extern state kshell_putenv(char *string);
extern state kshell_setenv(const char *name, const char *val, bool overwrite);
extern state kshell_unsetenv(const char *name);
extern state kshell_system(const char *cmdline);
extern state kshell_register(struct kshell_command *);
extern void kshell_unregister(struct kshell_command *);
extern void ksh_init(void);

#endif  /* CONFIG_KSHELL */
#endif  /* _KSHELL_H_ */