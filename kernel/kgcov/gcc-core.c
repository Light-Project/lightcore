/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kgcov.h>
#include <export.h>

static struct kgcov_info *kgcov_info_head;

/**
 * kgcov_info_filename - return info filename.
 * @info: profiling data set.
 */
const char *kgcov_info_filename(struct kgcov_info *info)
{
    return info->filename;
}

/**
 * kgcov_info_version - return info version.
 * @info: profiling data set.
 */
unsigned int kgcov_info_version(struct kgcov_info *info)
{
    return info->version;
}

/**
 * kgcov_info_link - link/add profiling data set to the list.
 * @info: profiling data set.
 */
void kgcov_info_link(struct kgcov_info *info)
{
    info->next = kgcov_info_head;
    kgcov_info_head = info;
}

/**
 * kgcov_info_unlink - unlink/remove profiling data set from the list.
 * @prev: previous profiling data set.
 * @info: profiling data set.
 */
void kgcov_info_unlink(struct kgcov_info *prev, struct kgcov_info *info)
{
    if (prev)
        prev->next = info->next;
    else
        kgcov_info_head = info->next;
}
