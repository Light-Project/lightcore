/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PRINTK_H_
#define _PRINTK_H_

#include <types.h>
#include <linkage.h>
#include <klevels.h>

#ifdef CONFIG_PRE_PRINTK
int pr_early(const char *str,...);
#else
static inline int pr_early(str,...) {}
#endif

asmlinkage __printf(1, 2) __cold
int printk(const char *fmt,...);

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
#define pr_debug(fmt, ...)  while(0);
#endif

#endif	/* _PRINTK_H_ */
