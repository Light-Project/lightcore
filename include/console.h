/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <types.h>
#include <bits.h>
#include <state.h>
#include <list.h>

#define CONSOLE_ENABLED BIT(0)
#define CONSOLE_BOOT    BIT(1)
#define CONSOLE_BUFFER  BIT(2)
#define CONSOLE_CONSDEV BIT(3)
#define CONSOLE_ANYTIME	BIT(4)

struct console {
    const char *name;
    unsigned int index;
    struct list_head list;
    struct console_ops *ops;
    uint8_t flags;
    void *data;
};

struct console_ops {
    void (*read)(struct console *, char *, unsigned);
    void (*write)(struct console *, const char *, unsigned);
    state (*startup)(struct console *);
    void (*shutdown)(struct console *);
};

/* pre console */
#ifdef CONFIG_PRE_PRINTK
void pre_console_write(const char *str, unsigned int len);
void pre_console_register(struct console *con);
void pre_printk_init(void);
#else
static inline void pre_console_write(const char *str, unsigned int len) {}
static inline void pre_console_register(struct console *con) {}
static inline void pre_printk_init(void) {}
#endif

/* console */
void console_write(const char *str, unsigned int len);
void console_register(struct console *);
void console_unregister(struct console *);
void console_init(void);

#endif  /* _CONSOLE_H_ */
