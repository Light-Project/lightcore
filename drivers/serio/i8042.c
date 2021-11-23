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

#define I8042_BUFFER_SIZE   16
#define I8042_MUX_PORTS     4
#define I8042_TIMEOUT       10000

struct i8042_device {
    struct irqchip_channel *kbdirq, *auxirq;
    struct serio_host kbd, *aux[I8042_MUX_PORTS];
    struct power_device power;

    resource_size_t base;
    bool mux_present;
    uint8_t initctrl, ctrl;
};

#define kbd_to_idev(pdev) \
    container_of(pdev, struct i8042_device, kbd)

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

static state i8042_hw_init(struct i8042_device *idev)
{
    unsigned int timeout = 10;
    uint8_t ctrl[2];
    state ret;

    while (--timeout && (timeout > 7 || ctrl[0] != ctrl[1])) {
        if ((ret = i8042_command(idev, I8042_CMD_CTL_RCTR, &ctrl[timeout++ % 2]))) {
            dev_err(idev->kbd.dev, "unable to read ctrl while initializing\n");
            return ret;
        }
        udelay(50);
    } if (!timeout) {
        dev_err(idev->kbd.dev, "unable to get stable ctrl\n");
        return -EIO;
    }

    idev->ctrl = idev->initctrl = ctrl[0];
    idev->ctrl |= I8042_CTRL_KBDDIS;
    idev->ctrl &= ~I8042_CTRL_KBDINT;

    if ((ret = i8042_command(idev, I8042_CMD_CTL_WCTR, &idev->ctrl))) {
        dev_err(idev->kbd.dev, "unable to write back ctrl while initializing\n");
        return ret;
    }

    return i8042_flush(idev) ? -EBUSY : -ENOERR;
}

static void i8042_hw_deinit(struct i8042_device *idev)
{
    if (i8042_command(idev, I8042_CMD_CTL_WCTR, &idev->initctrl))
        dev_err(idev->kbd.dev, "unable to recovery ctrl\n");
}

static state i8042_kbd_port_enable(struct i8042_device *idev)
{
    state ret;

    idev->ctrl &= ~I8042_CTRL_KBDDIS;
    idev->ctrl |= I8042_CTRL_KBDINT;

    if ((ret = i8042_command(idev, I8042_CMD_CTL_WCTR, &idev->ctrl))) {
        idev->ctrl &= ~I8042_CTRL_KBDINT;
        idev->ctrl |= I8042_CTRL_KBDDIS;
        return ret;
    }

    return -ENOERR;
}

static state i8042_aux_port_enable(struct i8042_device *idev)
{
    state ret;

    idev->ctrl &= ~I8042_CTRL_AUXDIS;
    idev->ctrl |= I8042_CTRL_AUXINT;

    if ((ret = i8042_command(idev, I8042_CMD_CTL_WCTR, &idev->ctrl))) {
        idev->ctrl &= ~I8042_CTRL_AUXINT;
        idev->ctrl |= I8042_CTRL_AUXDIS;
        return ret;
    }

    return -ENOERR;
}

static state i8042_mux_enable(struct i8042_device *idev)
{
    uint8_t para;
    int count;
    state ret;

    for (count = 0; count < I8042_MUX_PORTS; count++)
        if ((ret = i8042_command(idev, I8042_CMD_MUX_PFX + count, &para)) ||
            (ret = i8042_command(idev, I8042_CMD_AUX_ENABLE, &para)))
                return ret;

    return i8042_aux_port_enable(idev);
}

static state i8042_mux_mode(struct i8042_device *idev, bool mux, uint8_t *version)
{
    uint8_t para, val;
    state ret;

    i8042_flush(idev);

    /* internal loopback test */
    *version = para = 0xf0;
    if ((ret = i8042_command(idev, I8042_CMD_AUX_LOOP, version)) ||
        (ret = val != para ? -EIO : -ENOERR))
        return ret;

    *version = para = mux ? 0x56 : 0xf6;
    if ((ret = i8042_command(idev, I8042_CMD_AUX_LOOP, version)) ||
        (ret = val != para ? -EIO : -ENOERR))
        return ret;

    *version = para = mux ? 0xa4 : 0xa5;
    if ((ret = i8042_command(idev, I8042_CMD_AUX_LOOP, version)) ||
        (ret = val != para ? -EIO : -ENOERR))
        return ret;

    if (*version == 0xac)
        return -ENODEV;

    return -ENOERR;
}

static state i8042_mux_detect(struct i8042_device *idev)
{
    uint8_t version;
    state ret;

    if ((ret = i8042_mux_mode(idev, true, &version)))
        return ret;

    idev->ctrl |= I8042_CTRL_AUXDIS;
    idev->ctrl &= ~I8042_CTRL_AUXINT;

    if (i8042_command(idev, I8042_CMD_CTL_WCTR, &idev->ctrl)) {
        dev_err(idev->kbd.dev, "unable disable aux port\n");
        return -EIO;
    }

    dev_info(idev->kbd.dev, "detect mux controller, rev %d.%d\n",
             (version >> 4) & 0x0f, version & 0x0f);

    idev->mux_present = true;
    return -ENOERR;
}

static state i8042_kbd_write(struct serio_host *shost, uint8_t cmd)
{
    struct i8042_device *idev = device_get_pdata(shost->dev);

    if (!i8042_write_wait(idev))
        return -EBUSY;

    i8042_out(idev, I8042_DATA, cmd);
    return -ENOERR;
}

static state i8042_aux_write(struct serio_host *shost, uint8_t cmd)
{
    struct i8042_device *idev = device_get_pdata(shost->dev);
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
    .reset = i8042_power_reset,
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

    if ((status & I8042_STATUS_AUXDATA) && idev->mux_present) {

    } else {
        shost = status & I8042_STATUS_AUXDATA ? &idev->aux : &idev->kbd;
    }

    val = i8042_in(idev, I8042_DATA);
    return serio_interrupt(shost, val, NULL);
}

static state i8042_kbd_setup(struct platform_device *pdev)
{
    struct i8042_device *idev = platform_get_devdata(pdev);
    struct irqchip_channel *irqchip;
    state ret;

    /* request irq handle */
    irq_request(ATKBD_IRQ, 0, i8042_handle, idev, DRIVER_NAME);

    irqchip = dt_get_irqchip_channel_by_name(pdev->dt_node, "kbd");
    if (!irqchip) {
        dev_err(&pdev->dev, "unable to request irq\n");
        return -EINVAL;
    }

    idev->kbdirq = irqchip;
    irqchip_pass(irqchip);

    if ((ret = i8042_kbd_port_enable(idev)))
        return ret;

    /* Resister main port */
    idev->kbd.port.name = ATKBD_MATCH_ID;
    idev->kbd.ops = &i8042_kbd_ops;
    idev->kbd.dev = &pdev->dev;
    return serio_host_register(&idev->kbd);
}

static i8042_aux_create(struct platform_device *pdev, unsigned int nr)
{
    struct i8042_device *idev = platform_get_devdata(pdev);
    struct serio_host *aux;
    unsigned int count;

    aux = dev_kzalloc(&pdev->dev, sizeof(*idev->aux) * nr, GFP_KERNEL);
    if (!aux)
        return -ENOMEM;

    while (nr--) {
        idev->aux[nr]->port.name = PSMSE_MATCH_ID;
        idev->aux[nr]->ops = &i8042_aux_ops;
        idev->aux[nr]->dev = &pdev->dev;
    }
}

static i8042_aux_free(struct platform_device *pdev)
{
    struct i8042_device *idev = platform_get_devdata(pdev);
    unsigned int count;

    for (count = 0; count < I8042_MUX_PORTS; ++count) {
        if (!idev->aux[count])
            return;
        serio_host_unregister(idev->aux[count]);
    }
}

static state i8042_aux_setup(struct platform_device *pdev)
{
    struct i8042_device *idev = platform_get_devdata(pdev);
    struct irqchip_channel *irqchip;
    state ret;

    /* request irq handle */
    irq_request(ATKBD_IRQ, 0, i8042_handle, idev, DRIVER_NAME);

    irqchip = dt_get_irqchip_channel_by_name(pdev->dt_node, "aux");
    if (!irqchip) {
        dev_err(&pdev->dev, "unable to request irq\n");
        return -EINVAL;
    }

    idev->auxirq = irqchip;
    irqchip_pass(irqchip);

    if (i8042_mux_detect(idev)) {
        if ((ret = i8042_aux_port_enable(idev)))
            return ret;
    } else {

    }

    /* Resister aux port */
    return serio_host_register(&idev->aux);
}

static void i8042_irqfree(struct platform_device *pdev)
{
    struct i8042_device *idev = platform_get_devdata(pdev);

    if (idev->kbdirq)
        irqchip_channel_release(idev->kbdirq);

    if (idev->auxirq)
        irqchip_channel_release(idev->auxirq);
}

static state i8042_probe(struct platform_device *pdev, void *pdata)
{
    struct i8042_device *idev;
    state ret;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENODEV;

    idev = dev_kzalloc(&pdev->dev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;

    platform_set_devdata(pdev, idev);
    idev->base = platform_resource_start(pdev, 0);

    /* Detect and init i8042 device */
    if ((ret = i8042_selftest(idev)) ||
        (ret = i8042_hw_init(idev)))
        return ret;

    /* Detect i8042 device */
    if ((ret = i8042_kbd_setup(pdev)))
        goto err_kbd;

    if ((ret = i8042_aux_setup(pdev)))
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
    i8042_irqfree(pdev);
    return ret;
}

static void i8042_remove(struct platform_device *pdev)
{
    struct i8042_device *idev = platform_get_devdata(pdev);

    i8042_irqfree(pdev);
    i8042_hw_deinit(idev);
    serio_host_unregister(&idev->kbd);
    serio_host_unregister(&idev->aux);
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
    .remove = i8042_remove,
};

static state i8042_init(void)
{
    return platform_driver_register(&i8042_driver);
}
driver_initcall(i8042_init);
