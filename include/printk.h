/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PRINTK_H_
#define _PRINTK_H_

#include <types.h>
#include <stdarg.h>
#include <kern_levels.h>
#include <console.h>

#ifdef CONFIG_PRE_PRINTK
extern struct console *pre_console;
void pre_printk(const char *str,...);
#else
#define pre_printk(str,...)
#endif

int printk(const char *fmt,...);
int vsnprintf(char * buf, size_t n, const char * fmt, va_list ap);

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

/**
 * pr_emerg - Print an emergency-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_EMERG loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#define pr_emerg(fmt, ...) \
    printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)

/**
 * pr_alert - Print an alert-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_ALERT loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#define pr_alert(fmt, ...) \
    printk(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)

/**
 * pr_crit - Print a critical-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_CRIT loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#define pr_crit(fmt, ...) \
    printk(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)

/**
 * pr_err - Print an error-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_ERR loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#define pr_err(fmt, ...) \
    printk(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)

/**
 * pr_warn - Print a warning-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_WARNING loglevel. It uses pr_fmt()
 * to generate the format string.
 */
#define pr_warn(fmt, ...) \
    printk(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)

/**
 * pr_notice - Print a notice-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_NOTICE loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#define pr_notice(fmt, ...) \
    printk(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__)

/**
 * pr_info - Print an info-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_INFO loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#define pr_info(fmt, ...) \
    printk(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__)

/**
 * pr_debug - Print an debug-level message
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_DEBUG loglevel. It uses pr_fmt() to
 * generate the format string.
 */
#ifdef CONFIG_DYNAMIC_DEBUG
#define pr_debug(fmt, ...) \
    printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...)
#endif

/**
 * pr_cont - Continues a previous log message in the same line.
 * @fmt: format string
 * @...: arguments for the format string
 *
 * This macro expands to a printk with KERN_CONT loglevel. It should only be
 * used when continuing a log message with no newline ('\n') enclosed. Otherwise
 * it defaults back to KERN_DEFAULT loglevel.
 */
#define pr_cont(fmt, ...) \
    printk(KERN_ONT fmt, ##__VA_ARGS__)


#endif	/* _PRINTK_H_ */
