/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <device.h>
#include <bitflags.h>

enum console_flags {
    __CONSOLE_BOOT          = 0,
    __CONSOLE_ENABLED       = 1,
    __CONSOLE_CONSDEV       = 2,
    __CONSOLE_BUFFER        = 3,
    __CONSOLE_ANYTIME       = 4,
};

#define CONSOLE_BOOT        BIT(__CONSOLE_BOOT)
#define CONSOLE_ENABLED     BIT(__CONSOLE_ENABLED)
#define CONSOLE_CONSDEV     BIT(__CONSOLE_CONSDEV)
#define CONSOLE_BUFFER      BIT(__CONSOLE_BUFFER)
#define CONSOLE_ANYTIME     BIT(__CONSOLE_ANYTIME)

struct console {
    struct list_head list;
    struct console_ops *ops;
    struct device *device;
    const char *name;
    unsigned int index;
    unsigned long flags;
};

struct console_ops {
    unsigned int (*read)(struct console *con, char *buff, unsigned int len);
    void (*write)(struct console *con, const char *buff, unsigned int len);
    void (*sync)(struct console *con);
    state (*startup)(struct console *con);
    void (*shutdown)(struct console *con);
};

GENERIC_STRUCT_BITOPS(console, struct console, flags, false)
GENERIC_STRUCT_FLAG(console, struct console, flags, boot, __CONSOLE_BOOT)
GENERIC_STRUCT_FLAG(console, struct console, flags, enabled, __CONSOLE_ENABLED)
GENERIC_STRUCT_FLAG(console, struct console, flags, consdev, __CONSOLE_CONSDEV)
GENERIC_STRUCT_FLAG(console, struct console, flags, buffer, __CONSOLE_BUFFER)
GENERIC_STRUCT_FLAG(console, struct console, flags, anytime, __CONSOLE_ANYTIME)

/* early console */
#ifdef CONFIG_EARLYCON
extern unsigned int earlycon_read(char *buff, unsigned int len);
extern void earlycon_write(const char *buff, unsigned int len);
extern void earlycon_sync(void);
extern void earlycon_register(struct console *con);
extern void earlycon_init(void);
#else
static inline unsigned int earlycon_read(const char *buff, unsigned int len) {return 0;}
static inline void earlycon_write(const char *buff, unsigned int len) {}
static inline void earlycon_sync(void) {}
static inline void earlycon_register(struct console *con) {}
static inline void earlycon_init(void) {}
#endif

extern unsigned int console_read(char *str, unsigned int len);
extern void console_write(const char *str, unsigned int len);
extern void console_sync(void);

extern void console_register(struct console *con);
extern void console_unregister(struct console *con);
extern void console_init(void);

#endif  /* _CONSOLE_H_ */
