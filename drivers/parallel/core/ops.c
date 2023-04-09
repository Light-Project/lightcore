/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <driver/parallel.h>
#include <export.h>

#define GENERIC_PRALLEL_HOST_OPS(name, operation, type)     \
state name(struct parallel_host *host, type value)          \
{                                                           \
    struct parallel_ops *ops = host->ops;                   \
                                                            \
    if (!ops->operation)                                    \
        return -EOPNOTSUPP;                                 \
                                                            \
    return ops->operation(host, value);                     \
}

GENERIC_PRALLEL_HOST_OPS(parallel_host_data_read, data_read, uint8_t *)
GENERIC_PRALLEL_HOST_OPS(parallel_host_data_write, data_write, uint8_t)
GENERIC_PRALLEL_HOST_OPS(parallel_host_ctrl_read, ctrl_read, uint8_t *)
GENERIC_PRALLEL_HOST_OPS(parallel_host_ctrl_write, ctrl_write, uint8_t)
GENERIC_PRALLEL_HOST_OPS(parallel_host_status_read, status_read, uint8_t *)

EXPORT_SYMBOL(parallel_host_data_read);
EXPORT_SYMBOL(parallel_host_data_write);
EXPORT_SYMBOL(parallel_host_ctrl_read);
EXPORT_SYMBOL(parallel_host_ctrl_write);
EXPORT_SYMBOL(parallel_host_status_read);

#define GENERIC_PRALLEL_OPS(name, operation, type)          \
state name(struct parallel_device *pdev, type data)         \
{                                                           \
    struct parallel_host *host = pdev->host;                \
    return operation(host, data);                           \
}

GENERIC_PRALLEL_OPS(parallel_data_read, parallel_host_data_read, uint8_t *)
GENERIC_PRALLEL_OPS(parallel_data_write, parallel_host_data_write, uint8_t)
GENERIC_PRALLEL_OPS(parallel_ctrl_read, parallel_host_ctrl_read, uint8_t *)
GENERIC_PRALLEL_OPS(parallel_ctrl_write, parallel_host_ctrl_write, uint8_t)
GENERIC_PRALLEL_OPS(parallel_status_read, parallel_host_status_read, uint8_t *)

EXPORT_SYMBOL(parallel_data_read);
EXPORT_SYMBOL(parallel_data_write);
EXPORT_SYMBOL(parallel_ctrl_read);
EXPORT_SYMBOL(parallel_ctrl_write);
EXPORT_SYMBOL(parallel_status_read);
