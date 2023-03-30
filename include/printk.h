/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PRINTK_H_
#define _PRINTK_H_

#include <types.h>
#include <stdarg.h>
#include <klevels.h>

enum printk_index {
    PRIDX_CONSOLE = 0,
    PRIDX_DEFAULT,
    PRIDX_RECORD,
    PRIDX_NUM_MAX,
};

#define PRINTK_LOGLEVEL_RECORD CONFIG_PRINTK_LOGLEVEL_RECORD
#define PRINTK_LOGLEVEL_DEFAULT CONFIG_PRINTK_LOGLEVEL_DEFAULT

#define CONSOLE_LOGLEVEL_DEFAULT CONFIG_CONSOLE_LOGLEVEL_DEFAULT
#define CONSOLE_LOGLEVEL_DEBUG CONFIG_CONSOLE_LOGLEVEL_DEBUG
#define CONSOLE_LOGLEVEL_QUIET CONFIG_CONSOLE_LOGLEVEL_QUIET

extern unsigned int printk_levels[PRIDX_NUM_MAX];
#define printk_console printk_levels[PRIDX_CONSOLE]
#define printk_default printk_levels[PRIDX_DEFAULT]
#define printk_record printk_levels[PRIDX_RECORD]

#ifdef CONFIG_EARLYCON
extern int pr_early(const char *str, ...);
#else
static inline int pr_early(const char *str, ...)
{
    return 0;
}
#endif

extern unsigned int printk_level(const char *str, const char **endptr);
extern int vprintk(const char *fmt, va_list args);
extern __printf(1, 2) int printk(const char *fmt, ...);

/**
 * pr_fmt - used by the pr_*() macros to generate the printk format string
 * @fmt: format string passed from a pr_*() macro
 *
 * This macro can be used to generate a unified format string for pr_*()
 * macros. A common use is to prefix all pr_*() messages in a file with a common
 * string. For example, defining this at the top of a source file:
 *
 *        #define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
 *
 * would prefix all pr_info, pr_emerg... messages in the file with the module
 * name.
 */
#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

/*
 * Dummy printk for disabled debugging statements to use whilst maintaining
 * gcc's format checking.
 */
#define pr_none(fmt, ...) ({            \
    if (0)                              \
        printk(fmt, ##__VA_ARGS__);     \
    0;                                  \
})

/**
 * pr_emerg - Print an emergency-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_EMERG loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#if CONFIG_PRINTK_LOGLEVEL_MAX >= KLEVEL_EMERG
#define pr_emerg(fmt, ...) \
    printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)
#else
# define pr_emerg(fmt, ...) \
    pr_none(fmt, ##__VA_ARGS__)
#endif

/**
 * pr_alert - Print an alert-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_ALERT loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#if CONFIG_PRINTK_LOGLEVEL_MAX >= KLEVEL_ALERT
#define pr_alert(fmt, ...) \
    printk(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)
#else
# define pr_alert(fmt, ...) \
    pr_none(fmt, ##__VA_ARGS__)
#endif

/**
 * pr_crit - Print a critical-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_CRIT loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#if CONFIG_PRINTK_LOGLEVEL_MAX >= KLEVEL_CRIT
#define pr_crit(fmt, ...) \
    printk(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)
#else
# define pr_crit(fmt, ...) \
    pr_none(fmt, ##__VA_ARGS__)
#endif

/**
 * pr_err - Print an error-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_ERR loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#if CONFIG_PRINTK_LOGLEVEL_MAX >= KLEVEL_ERR
# define pr_err(fmt, ...) \
    printk(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)
#else
# define pr_err(fmt, ...) \
    pr_none(fmt, ##__VA_ARGS__)
#endif

/**
 * pr_warn - Print a warning-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_WARNING loglevel. It uses pr_fmt()
 * to generate the format string.
 */
#if CONFIG_PRINTK_LOGLEVEL_MAX >= KLEVEL_WARNING
# define pr_warn(fmt, ...) \
    printk(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)
#else
# define pr_warn(fmt, ...) \
    pr_none(fmt, ##__VA_ARGS__)
#endif

/**
 * pr_notice - Print a notice-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_NOTICE loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#if CONFIG_PRINTK_LOGLEVEL_MAX >= KLEVEL_NOTICE
# define pr_notice(fmt, ...) \
    printk(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__)
#else
# define pr_notice(fmt, ...) \
    pr_none(fmt, ##__VA_ARGS__)
#endif

/**
 * pr_info - Print an info-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_INFO loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#if CONFIG_PRINTK_LOGLEVEL_MAX >= KLEVEL_INFO
# define pr_info(fmt, ...) \
    printk(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__)
#else
# define pr_info(fmt, ...) \
    pr_none(fmt, ##__VA_ARGS__)
#endif

/**
 * pr_debug - Print an debug-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_DEBUG loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#ifdef CONFIG_DYNAMIC_DEBUG
# define pr_debug(fmt, ...) \
    printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#else
# define pr_debug(fmt, ...) \
    pr_none(fmt, ##__VA_ARGS__)
#endif

#endif	/* _PRINTK_H_ */
