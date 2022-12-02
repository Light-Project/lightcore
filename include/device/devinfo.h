/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DEVICE_DEVINFO_H_
#define _DEVICE_DEVINFO_H_

#include <printk.h>

#define dev_emerg(dev, fmt, ...) \
    pr_emerg("%s: "fmt, (dev)->name, ##__VA_ARGS__)

#define dev_alert(dev, fmt, ...) \
    pr_alert("%s: "fmt, (dev)->name, ##__VA_ARGS__)

#define dev_crit(dev, fmt, ...) \
    pr_crit("%s: "fmt, (dev)->name, ##__VA_ARGS__)

#define dev_err(dev, fmt, ...) \
    pr_err("%s: "fmt, (dev)->name, ##__VA_ARGS__)

#define dev_warn(dev, fmt, ...) \
    pr_warn("%s: "fmt, (dev)->name, ##__VA_ARGS__)

#define dev_notice(dev, fmt, ...) \
    pr_notice("%s: "fmt, (dev)->name, ##__VA_ARGS__)

#define dev_info(dev, fmt, ...) \
    pr_info("%s: "fmt, (dev)->name, ##__VA_ARGS__)

#define dev_debug(dev, fmt, ...) \
    pr_debug("%s: "fmt, (dev)->name, ##__VA_ARGS__)

#define dev_none(dev, fmt, ...) \
    pr_none("%s: "fmt, (dev)->name, ##__VA_ARGS__)

#endif  /* _DEVICE_DEVINFO_H_ */
