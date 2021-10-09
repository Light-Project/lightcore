/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i8042"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <kernel.h>
#include <kmalloc.h>
#include <initcall.h>
#include <irq.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/power.h>
#include <driver/serio.h>
#include <driver/serio/i8042.h>
#include <driver/input/atkbd.h>
#include <driver/input/psmse.h>
#include <printk.h>

#include <asm/delay.h>
#include <asm/io.h>

#define I8042_TIMEOUT   10000

struct i8042_device {
    struct irqchip_channel *irqchip;
    struct serio_host kbd, aux;
    struct power_device power;

    resource_size_t base;
    uint8_t ctrl;
};

#define kbd_to_idev(pdev) \
    container_of(pdev, struct i8042_device, kbd)

#define aux_to_idev(pdev) \
    container_of(pdev, struct i8042_device, aux)

#define power_to_idev(pdev) \
    container_of(pdev, struct i8042_device, power)

static __always_inline uint8_t
i8042_in(struct i8042_device *idev, int reg)
{
    return inb(idev->base + reg);
}

static __always_inline void
i8042_out(struct i8042_device *idev, int reg, uint8_t val)
{
    outb(idev->base + reg, val);
}

static bool i8042_wait(struct i8042_device *idev, uint8_t mask, uint8_t pass)
{
    int timeout = I8042_TIMEOUT;
    uint8_t val;

    while (timeout--) {
        val = i8042_in(idev, I8042_STATUS);
        if ((val & mask) == pass)
            break;
        udelay(50);
    }

    return !!timeout;
}

static inline bool i8042_read_wait(struct i8042_device *idev)
{
    return i8042_wait(idev, I8042_STATUS_OBF, I8042_STATUS_OBF);
}

static inline bool i8042_write_wait(struct i8042_device *idev)
{
    return i8042_wait(idev, I8042_STATUS_IBF, 0);
}

static inline bool i8042_flush(struct i8042_device *idev)
{
    return i8042_wait(idev, I8042_STATUS_OBF, 0);
}

static state i8042_command(struct i8042_device *idev, uint16_t cmd, uint8_t *para)
{
    int receive = (cmd >> 8) & 0x0f;
    int send = (cmd >> 12) & 0x0f;
    uint8_t *buff = para;

    if (!i8042_write_wait(idev))
        return -EBUSY;
    i8042_out(idev, I8042_CMD, cmd);

    while (send--) {
        if (!i8042_write_wait(idev))
            return -EBUSY;
        i8042_out(idev, I8042_DATA, *para++);
    }

    while (receive--) {
        if (!i8042_read_wait(idev))
            return -EBUSY;
        *buff++ = i8042_in(idev, I8042_DATA);
    }

    return -ENOERR;
}

static state i8042_selftest(struct i8042_device *idev)
{
    uint8_t para;
    state ret;

    if ((ret = i8042_command(idev, I8042_CMD_CTL_TEST, &para)) ||
        (ret = para != 0x55 ? -EIO : -ENOERR)) {
        dev_err(idev->kbd.dev, "self test failed: code %x\n", para);
        return ret;
    }

    if ((ret = i8042_command(idev, I8042_CMD_KBD_TEST, &para)) ||
        (ret = para != 0x00 ? -EIO : -ENOERR)) {
        dev_err(idev->kbd.dev, "keyboard test failed: code %x\n", para);
        return ret;
    }

    if ((ret = i8042_command(idev, I8042_CMD_AUX_TEST, &para)) ||
        (ret = para != 0x00 ? -EIO : -ENOERR)) {
        dev_err(idev->kbd.dev, "mouse test failed: code %x\n", para);
        return ret;
    }

    return -ENOERR;
}

static state i8042_kbd_enable(struct i8042_device *idev)
{
    idev->ctrl &= ~I8042_CTRL_KBDDIS;
    idev->ctrl |= I8042_CTRL_KBDINT;

    if (i8042_command(idev, I8042_CMD_CTL_WCTR, &idev->ctrl)) {
        idev->ctrl &= ~I8042_CTRL_KBDINT;
        idev->ctrl |= I8042_CTRL_KBDDIS;
        return -EIO;
    }

    return -ENOERR;
}

static state i8042_aux_enable(struct i8042_device *idev)
{
    idev->ctrl &= ~I8042_CTRL_AUXDIS;
    idev->ctrl |= I8042_CTRL_AUXINT;

    if (i8042_command(idev, I8042_CMD_CTL_WCTR, &idev->ctrl)) {
        idev->ctrl &= ~I8042_CTRL_AUXINT;
        idev->ctrl |= I8042_CTRL_AUXDIS;
        return -EIO;
    }

    return -ENOERR;
}

static state i8042_kbd_write(struct serio_host *shost, uint8_t cmd)
{
    struct i8042_device *idev = kbd_to_idev(shost);

    if (!i8042_write_wait(idev))
        return -EBUSY;

    i8042_out(idev, I8042_DATA, cmd);
    return -ENOERR;
}

static state i8042_aux_write(struct serio_host *shost, uint8_t cmd)
{
    struct i8042_device *idev = aux_to_idev(shost);
    return i8042_command(idev, I8042_CMD_AUX_SEND, &cmd);
}

static void i8042_power_reset(struct power_device *pdev)
{
    struct i8042_device *idev = power_to_idev(pdev);
    int timeout = 100;

    while (timeout--) {
        i8042_write_wait(idev);
        udelay(50);
        i8042_out(idev, I8042_CMD, 0xfe);
        udelay(50);
    }
}

static struct serio_ops i8042_kbd_ops = {
    .write = i8042_kbd_write,
};

static struct serio_ops i8042_aux_ops = {
    .write = i8042_aux_write,
};

static struct power_ops i8042_power_ops = {
    .restart = i8042_power_reset,
};

static irqreturn_t i8042_handle(irqnr_t vector, void *data)
{
    struct i8042_device *idev = data;
    struct serio_host *shost;
    uint8_t status, val;

    status = i8042_in(idev, I8042_STATUS);
    if (!(status & I8042_STATUS_OBF)) {
        dev_err(idev->kbd.dev, "handle without data\n");
        return IRQ_RET_NONE;
    }

    shost = (status & I8042_STATUS_AUXDATA) ?
            &idev->aux : &idev->kbd;

    val = i8042_in(idev, I8042_DATA);
    return serio_interrupt(shost, val, NULL);
}

static state i8042_probe(struct platform_device *pdev, void *pdata)
{
    struct i8042_device *idev;
    struct irqchip_channel *irqchip;
    state ret;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENODEV;

    idev = dev_kzalloc(&pdev->dev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;

    idev->base = platform_resource_start(pdev, 0);
    idev->kbd.dev = &pdev->dev;
    idev->aux.dev = &pdev->dev;

    /* Detect i8042 device */
    if (!i8042_flush(idev) || i8042_selftest(idev))
        return -ENODEV;

    i8042_kbd_enable(idev);
    i8042_aux_enable(idev);

    /* Request irq channel */
    irqchip = dt_get_irqchip_channel(pdev->dt_node, 0);
    if (!irqchip) {
        dev_err(&pdev->dev, "unable to request irq\n");
        return -EINVAL;
    }

    idev->irqchip = irqchip;
    irqchip_pass(irqchip);
    irq_request(I8042_IRQ, 0, i8042_handle, idev, DRIVER_NAME);

    /* Resister main port */
    idev->kbd.ops = &i8042_kbd_ops;
    if ((ret = serio_host_register(&idev->kbd)))
        goto err_kbd;

    /* Resister aux port */
    idev->aux.ops = &i8042_aux_ops;
    if ((ret = serio_host_register(&idev->aux)))
        goto err_aux;

    /* Resister power device */
    idev->power.dev = &pdev->dev;
    idev->power.ops = &i8042_power_ops;
    if ((ret = power_register(&idev->power)))
        goto err_pwr;

    return -ENOERR;
err_pwr:
    serio_host_unregister(&idev->aux);
err_aux:
    serio_host_unregister(&idev->kbd);
err_kbd:
    irqchip_channel_release(irqchip);
    return ret;
}

static struct dt_device_id i8042_ids[] = {
    { .compatible = "intel,i8042" },
    { }, /* NULL */
};

static struct platform_driver i8042_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i8042_ids,
    .probe = i8042_probe,
};

static state i8042_init(void)
{
    return platform_driver_register(&i8042_driver);
}
driver_initcall(i8042_init);
