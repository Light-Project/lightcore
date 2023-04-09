/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <spinlock.h>
#include <textsearch.h>
#include <export.h>

static RW_LOCK(textsearch_lock);
static LIST_HEAD(textsearch_algos);

static struct ts_algorithm *algo_find(const char *name)
{
    struct ts_algorithm *algo;

    list_for_each_entry(algo, &textsearch_algos, list) {
        if (!strcmp(algo->name, name))
            return algo;
    }

    return NULL;
}

state textsearch_register(struct ts_algorithm *algo)
{
    state retval;

    if (!algo->name || !algo->find || !algo->prepare ||
        !algo->destroy || !algo->pattern_get || !algo->pattern_len)
        return -EINVAL;

    write_lock(&textsearch_lock);
    if (algo_find(algo->name))
        retval = -EALREADY;
    else {
        list_add(&textsearch_algos, &algo->list);
        retval = -ENOERR;
    }
    write_unlock(&textsearch_lock);

    return retval;
}

state textsearch_unregister(struct ts_algorithm *algo)
{
    state retval;

    write_lock(&textsearch_lock);
    if (algo_find(algo->name))
        retval = -EALREADY;
    else {
        list_del(&algo->list);
        retval = -ENOERR;
    }
    write_unlock(&textsearch_lock);

    return retval;
}
EXPORT_SYMBOL(textsearch_unregister);

struct ts_context *textsearch_prepare(const char *name, const void *pattern,
                                      size_t len, gfp_t gfp, unsigned long flags)
{
    struct ts_algorithm *algo;
    struct ts_context *tsc;

    read_lock(&textsearch_lock);
    algo = algo_find(name);
    read_unlock(&textsearch_lock);

    if (!algo)
        return NULL;

    tsc = algo->prepare(pattern, len, gfp, flags);
    if (IS_INVAL(tsc))
        return tsc;

    tsc->algo = algo;
    return tsc;
}
EXPORT_SYMBOL(textsearch_prepare);
