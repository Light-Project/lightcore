/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "parallel-gpio"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/gpio.h>
#include <driver/parallel.h>
#include <dt-bindings/gpio/parallel.h>

enum parallel_reg_type {
    PARALLEL_REG_DATA,
    PARALLEL_REG_STATUS,
    PARALLEL_REG_CTRL,
};

struct parallel_pin_desc {
    enum parallel_reg_type type;
    unsigned int index;
    enum gpio_direction dire;
    bool inverted;
};

static const struct parallel_pin_desc parallel_desc[] = {
    [GPIO_PARALLEL_STROBE] = {
        .type = PARALLEL_REG_CTRL,
        .index = 0,
        .dire = GPIO_DIRECTION_DRAIN,
        .inverted = true,
    },
    [GPIO_PARALLEL_D0] = {
        .type = PARALLEL_REG_DATA,
        .index = 0,
        .dire = GPIO_DIRECTION_PUSH,
        .inverted = false,
    },
    [GPIO_PARALLEL_D1] = {
        .type = PARALLEL_REG_DATA,
        .index = 1,
        .dire = GPIO_DIRECTION_PUSH,
        .inverted = false,
    },
    [GPIO_PARALLEL_D2] = {
        .type = PARALLEL_REG_DATA,
        .index = 2,
        .dire = GPIO_DIRECTION_PUSH,
        .inverted = false,
    },
    [GPIO_PARALLEL_D3] = {
        .type = PARALLEL_REG_DATA,
        .index = 3,
        .dire = GPIO_DIRECTION_PUSH,
        .inverted = false,
    },
    [GPIO_PARALLEL_D4] = {
        .type = PARALLEL_REG_DATA,
        .index = 4,
        .dire = GPIO_DIRECTION_PUSH,
        .inverted = false,
    },
    [GPIO_PARALLEL_D5] = {
        .type = PARALLEL_REG_DATA,
        .index = 5,
        .dire = GPIO_DIRECTION_PUSH,
        .inverted = false,
    },
    [GPIO_PARALLEL_D6] = {
        .type = PARALLEL_REG_DATA,
        .index = 6,
        .dire = GPIO_DIRECTION_PUSH,
        .inverted = false,
    },
    [GPIO_PARALLEL_D7] = {
        .type = PARALLEL_REG_DATA,
        .index = 7,
        .dire = GPIO_DIRECTION_PUSH,
        .inverted = false,
    },
    [GPIO_PARALLEL_ACK] = {
        .type = PARALLEL_REG_STATUS,
        .index = 6,
        .dire = GPIO_DIRECTION_INPUT,
        .inverted = false,
    },
    [GPIO_PARALLEL_BUSY] = {
        .type = PARALLEL_REG_STATUS,
        .index = 7,
        .dire = GPIO_DIRECTION_INPUT,
        .inverted = true,
    },
    [GPIO_PARALLEL_PAPEROUT] = {
        .type = PARALLEL_REG_STATUS,
        .index = 6,
        .dire = GPIO_DIRECTION_INPUT,
        .inverted = false,
    },
    [GPIO_PARALLEL_SELIN] = {
        .type = PARALLEL_REG_STATUS,
        .index = 4,
        .dire = GPIO_DIRECTION_INPUT,
        .inverted = false,
    },
    [GPIO_PARALLEL_AUTOFD] = {
        .type = PARALLEL_REG_CTRL,
        .index = 1,
        .dire = GPIO_DIRECTION_DRAIN,
        .inverted = true,
    },
    [GPIO_PARALLEL_ERROR] = {
        .type = PARALLEL_REG_STATUS,
        .index = 3,
        .dire = GPIO_DIRECTION_INPUT,
        .inverted = false,
    },
    [GPIO_PARALLEL_INIT] = {
        .type = PARALLEL_REG_CTRL,
        .index = 2,
        .dire = GPIO_DIRECTION_DRAIN,
        .inverted = false,
    },
    [GPIO_PARALLEL_SELECT] = {
        .type = PARALLEL_REG_CTRL,
        .index = 3,
        .dire = GPIO_DIRECTION_DRAIN,
        .inverted = true,
    },
};

struct parallel_gpio_device {
    struct gpio_device gpio;
    struct parallel_device *pdev;
    uint8_t data, ctrl;
};

#define gpio_to_parallel_gpio(ptr) \
    container_of(ptr, struct parallel_gpio_device, gpio)

static state parallel_gpio_value_get(struct gpio_device *gdev, unsigned int port, bool *val)
{
    struct parallel_gpio_device *pgpio = gpio_to_parallel_gpio(gdev);
    uint8_t value;
    state ret;

    if (port >= ARRAY_SIZE(parallel_desc))
        return -EINVAL;

    switch (parallel_desc[port].type) {
        case PARALLEL_REG_DATA:
            ret = parallel_data_read(pgpio->pdev, &value);
            value = (value >> parallel_desc[port].index) & 0x01;
            *val = parallel_desc[port].inverted ? !value : !!value;
            break;

        case PARALLEL_REG_STATUS:
            ret = parallel_status_read(pgpio->pdev, &value);
            value = (value >> parallel_desc[port].index) & 0x01;
            *val = parallel_desc[port].inverted ? !value : !!value;
            break;

        case PARALLEL_REG_CTRL:
            ret = parallel_ctrl_read(pgpio->pdev, &value);
            value = (value >> parallel_desc[port].index) & 0x01;
            *val = parallel_desc[port].inverted ? !value : !!value;
            break;

        default:
            return -EOPNOTSUPP;
    }

    return ret;
}

static state parallel_gpio_value_set(struct gpio_device *gdev, unsigned int port, bool val)
{
    struct parallel_gpio_device *pgpio = gpio_to_parallel_gpio(gdev);
    uint8_t value;
    state ret;

    if (port >= ARRAY_SIZE(parallel_desc))
        return -EINVAL;

    switch (parallel_desc[port].type) {
        case PARALLEL_REG_DATA:
            value = pgpio->data;
            value = (parallel_desc[port].inverted ? !val : val) ?
                value | BIT(parallel_desc[port].index) :
                value & ~BIT(parallel_desc[port].index);
            ret = parallel_data_write(pgpio->pdev, (pgpio->data = value));
            break;

        case PARALLEL_REG_CTRL:
            value = pgpio->ctrl;
            value = (parallel_desc[port].inverted ? !val : val) ?
                value | BIT(parallel_desc[port].index) :
                value & ~BIT(parallel_desc[port].index);
            ret = parallel_ctrl_write(pgpio->pdev, (pgpio->ctrl = value));
            break;

        default:
            return -EOPNOTSUPP;
    }

    return ret;
}

static state parallel_gpio_direction_get(struct gpio_device *gdev, unsigned int port,
                                         enum gpio_direction *dir)
{
    if (port >= ARRAY_SIZE(parallel_desc))
        return -EINVAL;

    *dir = parallel_desc[port].dire;

    return -ENOERR;
}

static state parallel_gpio_direction_set(struct gpio_device *gdev, unsigned int port,
                                         enum gpio_direction dir)
{
    if (port >= ARRAY_SIZE(parallel_desc))
        return -EINVAL;

    if (dir != parallel_desc[port].dire)
        return -EOPNOTSUPP;

    return -ENOERR;
}

static state parallel_gpio_pull_get(struct gpio_device *gdev, unsigned int port,
                                    enum gpio_pull *pull)
{
    if (port >= ARRAY_SIZE(parallel_desc))
        return -EINVAL;

    if (parallel_desc[port].dire == GPIO_DIRECTION_PUSH)
        *pull = GPIO_PULL_FLOAT;
    else
        *pull = GPIO_PULL_UP;

    return -ENOERR;
}

static state parallel_gpio_pull_set(struct gpio_device *gdev, unsigned int port,
                                    enum gpio_pull pull)
{
    bool result;

    if (port >= ARRAY_SIZE(parallel_desc))
        return -EINVAL;

    if (parallel_desc[port].dire == GPIO_DIRECTION_PUSH)
        result = pull != GPIO_PULL_FLOAT;
    else
        result = pull != GPIO_PULL_UP;

    return result ? -EOPNOTSUPP : -ENOERR;
}

static state parallel_gpio_dt_xlate(struct gpio_device *gdev, const struct dt_phandle_args *args,
                                    uint32_t *index, uint32_t *flags)
{
    uint32_t pinnum;

    pinnum = args->args[0];
    if (pinnum >gdev->gpio_num)
        return -EINVAL;

    if (index)
        *index = pinnum;

    if (flags)
        *flags = args->args[1];

    return -ENOERR;
}

static struct gpio_ops parallel_gpio_ops = {
    .value_get = parallel_gpio_value_get,
    .value_set = parallel_gpio_value_set,
    .direction_get = parallel_gpio_direction_get,
    .direction_set = parallel_gpio_direction_set,
    .pull_get = parallel_gpio_pull_get,
    .pull_set = parallel_gpio_pull_set,
    .dt_xlate = parallel_gpio_dt_xlate,
    .dt_gpio_cells = 2,
};

static state parallel_gpio_hwinit(struct gpio_device *gdev)
{
    unsigned int count;
    state ret;

    for (count = 0; count < ARRAY_SIZE(parallel_desc); ++count) {
        switch (parallel_desc[count].type) {
            case PARALLEL_REG_DATA:
                ret = parallel_gpio_value_set(gdev, count, 1);
                if (ret)
                    return ret;
                break;

            case PARALLEL_REG_CTRL:
                ret = parallel_gpio_value_set(gdev, count, 1);
                if (ret)
                    return ret;
                break;

            default:
                break;
        }
    }

    return -ENOERR;
}

static state parallel_gpio_probe(struct parallel_device *pdev, const void *pdata)
{
    struct parallel_gpio_device *pgpio;
    state ret;

    pgpio = parallel_kzalloc(pdev, sizeof(*pgpio), GFP_KERNEL);
    if (!pgpio)
        return -ENOMEM;

    pgpio->pdev = pdev;
    pgpio->gpio.dev = &pdev->dev;
    pgpio->gpio.dt_node = pdev->dt_node;
    pgpio->gpio.ops = &parallel_gpio_ops;
    pgpio->gpio.gpio_base = UINT_MAX;
    pgpio->gpio.gpio_num = ARRAY_SIZE(parallel_desc);
    parallel_set_devdata(pdev, pgpio);

    ret = parallel_gpio_hwinit(&pgpio->gpio);
    if (ret) {
        parallel_err(pdev, "failed to initialize parallel hardware\n");
        return ret;
    }

    return gpio_register(&pgpio->gpio);
}

static const struct dt_device_id parallel_gpio_ids[] = {
    { .compatible = "parallel-gpio" },
    { }, /* NULL */
};

static struct parallel_driver parallel_gpio_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = parallel_gpio_ids,
    .probe = parallel_gpio_probe,
};

static state parallel_init(void)
{
    return parallel_driver_register(&parallel_gpio_driver);
}
driver_initcall(parallel_init);
