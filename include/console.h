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
    struct list_head list;
    const char *name;
    unsigned int index;
    uint8_t flags;
    struct console_ops *ops;
};

struct console_ops {
    unsigned int (*read)(struct console *, char *buff, unsigned int len);
    void (*write)(struct console *, const char *buff, unsigned int len);
    state (*startup)(struct console *);
    void (*shutdown)(struct console *);
};

/* early console */
#ifdef CONFIG_PRE_CONSOLE
extern unsigned int pre_console_read(char *buff, unsigned int len);
extern void pre_console_write(const char *buff, unsigned int len);
extern void pre_console_register(struct console *con);
extern void pre_console_init(void);
#else
static inline unsigned int pre_console_read(const char *buff, unsigned int len)
{
    return 0;
}
static inline void pre_console_write(const char *buff, unsigned int len) {}
static inline void pre_console_register(struct console *con) {}
static inline void pre_console_init(void) {}
#endif

/* console */
extern unsigned int console_read(char *str, unsigned int len);
extern void console_write(const char *str, unsigned int len);
extern void console_register(struct console *);
extern void console_unregister(struct console *);
extern void console_init(void);

#endif  /* _CONSOLE_H_ */
