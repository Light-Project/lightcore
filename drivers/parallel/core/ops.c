/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <driver/parallel.h>
#include <export.h>

state parallel_host_data_read(struct parallel_host *host, uint8_t *data)
{
    struct parallel_ops *ops = host->ops;

    if (!ops->data_read)
        return -EOPNOTSUPP;

    return ops->data_read(host, data);
}
EXPORT_SYMBOL(parallel_host_data_read);

state parallel_host_data_write(struct parallel_host *host, uint8_t data)
{
    struct parallel_ops *ops = host->ops;

    if (!ops->data_write)
        return -EOPNOTSUPP;

    return ops->data_write(host, data);
}
EXPORT_SYMBOL(parallel_host_data_write);

state parallel_data_read(struct parallel_device *pdev, uint8_t *data)
{
    struct parallel_host *host = pdev->host;
    return parallel_host_data_read(host, data);
}
EXPORT_SYMBOL(parallel_data_read);

state parallel_data_write(struct parallel_device *pdev, uint8_t data)
{
    struct parallel_host *host = pdev->host;
    return parallel_host_data_write(host, data);
}
EXPORT_SYMBOL(parallel_data_write);
