/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "i2c-dt"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/i2c.h>
#include <driver/dt.h>
#include <dt-bindings/i2c.h>

static state i2c_dt_device_prop(struct i2c_device *idev, struct dt_node *node)
{
    struct i2c_host *host = idev->host;
    uint32_t value;
    state retval;

    retval = dt_attribute_read_u32(node, "reg", &value);
    if (retval) {
        dev_err(host->dev, "invalid reg on %p\n", node);
        return retval;
    }

    if (value & I2C_TEN_BIT_ADDRESS) {
        value &= ~I2C_TEN_BIT_ADDRESS;
        idev->flags |= I2C_DEVICE_TEN;
    }

    if (value & I2C_OWN_SLAVE_ADDRESS) {
        value &= ~I2C_OWN_SLAVE_ADDRESS;
        idev->flags |= I2C_DEVICE_SLAVE;
    }

    if (dt_attribute_read_bool(node, "host-notify"))
        idev->flags |= I2C_DEVICE_NOTIFY;

    if (dt_attribute_read_bool(node, "wakeup-source"))
        idev->flags |= I2C_DEVICE_WAKE;

    idev->addr = value;
    idev->dev.name = node->path;
    idev->dev.fwnode = &node->fwnode;

    return -ENOERR;
}

static state dt_register_i2c_device(struct i2c_host *host, struct dt_node *node)
{
    struct i2c_device *idev;
    state retval;

    idev = i2c_device_alloc(host, NULL);
    if (unlikely(!idev)) {
        return -ENOMEM;
    }

    idev->dt_node = node;
    retval = i2c_dt_device_prop(idev, node);
    if (retval)
        goto failed;

    retval = i2c_device_register(idev);
    if (retval)
        goto failed;

    return -ENOERR;

failed:
    i2c_device_free(idev);
    return retval;
}

void dt_populate_i2c(struct i2c_host *host)
{
    struct dt_node *walk, *node;
    state ret;

    if (!fwnode_test_dtree(host->dev->fwnode))
        return;

    node = device_node_container(host->dev);
    dt_for_each_child(walk, node) {
        ret = dt_register_i2c_device(host, walk);
        if (ret) {
            pr_warn("failed to create device for %p\n", walk);
        }
    }
}
