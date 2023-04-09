/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <kobject.h>
#include <string.h>
#include <sections.h>
#include <kmalloc.h>
#include <export.h>

state kobject_set_name_vargs(struct kobject *kobj, const char *fmt, va_list args)
{
    const char *buff;

    if (unlikely(kobj->name && !fmt))
        return -ENOERR;

    buff = vasprintf_const(fmt, args);
    if (unlikely(!buff))
        return -ENOMEM;

    if (strchr(buff, '/')) {
		char *replace;

        if (!in_rodata_section(buff))
            replace = (char *)buff;
        else {
            replace = strdup(buff);
            kfree(buff);
        }

		strchreplace(replace, '!', '/');
        buff = replace;
    }

    if (!in_rodata_section(kobj->name))
        kfree(kobj->name);
    kobj->name = buff;

    return -ENOERR;
}
EXPORT_SYMBOL(kobject_set_name_vargs);

state kobject_set_name(struct kobject *kobj, const char *fmt, ...)
{
    va_list args;
    state retval;

    va_start(args, fmt);
    retval = kobject_set_name_vargs(kobj, fmt, args);
    va_end(args);

    return retval;
}
EXPORT_SYMBOL(kobject_set_name);
