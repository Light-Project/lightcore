/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <kgcov.h>
#include <mutex.h>
#include <printk.h>
#include <export.h>

void __gcov_init(struct kgcov_info *info)
{
    static unsigned int gcov_version;

    mutex_lock(&kgcov_lock);
    if (unlikely(!gcov_version)) {
        gcov_version = kgcov_info_version(info);
        pr_info("gcov version magic: %#x\n", gcov_version);
    }
    kgcov_info_link(info);
    mutex_unlock(&kgcov_lock);
}
EXPORT_SYMBOL(__gcov_init);

void __gcov_flush(void)
{
    /* Nothing */
}
EXPORT_SYMBOL(__gcov_flush);

void __gcov_merge_add(kgcov_t *cnts, unsigned int cntnum)
{
    /* Nothing */
}
EXPORT_SYMBOL(__gcov_merge_add);

void __gcov_merge_single(kgcov_t *cnts, unsigned int cntnum)
{
    /* Nothing */
}
EXPORT_SYMBOL(__gcov_merge_single);

void __gcov_merge_delta(kgcov_t *cnts, unsigned int cntnum)
{
    /* Nothing */
}
EXPORT_SYMBOL(__gcov_merge_delta);

void __gcov_merge_ior(kgcov_t *cnts, unsigned int cntnum)
{
    /* Nothing */
}
EXPORT_SYMBOL(__gcov_merge_ior);

void __gcov_merge_time_profile(kgcov_t *cnts, unsigned int cntnum)
{
    /* Nothing */
}
EXPORT_SYMBOL(__gcov_merge_time_profile);

void __gcov_merge_icall_topn(kgcov_t *cnts, unsigned int cntnum)
{
    /* Nothing */
}
EXPORT_SYMBOL(__gcov_merge_icall_topn);

void __gcov_exit(void)
{
    /* Nothing */
}
EXPORT_SYMBOL(__gcov_exit);
